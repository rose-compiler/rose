#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "DisassemblerX86.h"

#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "SageBuilderAsm.h"
#include "integerOps.h"
#include "stringify.h"
#include "DispatcherX86.h"
#include "BinaryUnparserX86.h"

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {


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
 * DisassemblerX86 primary methods, mostly defined by the superclass.
 *========================================================================================================================*/

bool
DisassemblerX86::canDisassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if (isSgAsmDOSFileHeader(header))
        return 2==wordSizeBytes();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family)
        return 4==wordSizeBytes();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family)
        return 8==wordSizeBytes();
    return false;
}

Unparser::BasePtr
DisassemblerX86::unparser() const {
    return Unparser::X86::instance();
}

void
DisassemblerX86::init(size_t wordsize)
{
    /* The default register dictionary.  If a register dictionary is specified in an SgAsmInterpretation, then that one will be
     * used instead of the default we set here. */
    const RegisterDictionary *regdict = NULL;
    size_t addrWidth=0;
    switch (wordsize) {
        case 2:
            name("i286");
            addrWidth = 16;
            insnSize = x86_insnsize_16;
#if 0 // [Robb P. Matzke 2015-06-23]
            regdict = RegisterDictionary::dictionary_i286();
#else
            // A word size of 2 bytes doesn't necessarily mean 80286. E.g., $ROSE/binaries/samples/exefmt.exe has a header that
            // advertises architecture ISA_IA32_Family with a word size of 2 and which contains an occasional 32-bit floating
            // point instruction, although perhaps because of disassembling data with a disassembler that understands 32-bit op
            // codes.
            regdict = RegisterDictionary::dictionary_i386_387();
#endif
            REG_IP = regdict->findOrThrow("ip");
            REG_SP = regdict->findOrThrow("sp");
            REG_SS = regdict->findOrThrow("ss");
            REG_SF = regdict->findOrThrow("bp");
            break;
        case 4:
            name("i386");
            addrWidth = 32;
            insnSize = x86_insnsize_32;
            regdict = RegisterDictionary::dictionary_pentium4();
            REG_IP = regdict->findOrThrow("eip");
            REG_SP = regdict->findOrThrow("esp");
            REG_SS = regdict->findOrThrow("ss");
            REG_SF = regdict->findOrThrow("ebp");
            callingConventions(CallingConvention::dictionaryX86());
            break;
        case 8:
            name("amd64");
            addrWidth = 64;
            insnSize = x86_insnsize_64;
            regdict = RegisterDictionary::dictionary_amd64();
            REG_IP = regdict->findOrThrow("rip");
            REG_SP = regdict->findOrThrow("rsp");
            REG_SS = regdict->findOrThrow("ss");
            REG_SF = regdict->findOrThrow("rbp");
            callingConventions(CallingConvention::dictionaryAmd64());
            break;
        default:
            ASSERT_not_reachable("instruction must be 2, 4, or 8 bytes");
    }
    InstructionSemantics2::DispatcherX86Ptr d = InstructionSemantics2::DispatcherX86::instance(addrWidth, regdict);
    d->set_register_dictionary(regdict);                // so register cache is initialized
    p_proto_dispatcher = d; 

    registerDictionary(regdict);
    wordSizeBytes(wordsize);
    byteOrder(ByteOrder::ORDER_LSB);

    /* Not actually necessary because we'll call it before each instruction. We call it here just to initialize all the data
     * members to reasonable values for debugging. */
    startInstruction(0, NULL, 0);
}

SgAsmInstruction *
DisassemblerX86::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors)
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
    unsigned char temp[16];
    size_t tempsz = map->at(start_va).limit(sizeof temp).require(MemoryMap::EXECUTABLE).read(temp).size();

    /* Disassemble the instruction */
    startInstruction(start_va, temp, tempsz);
    SgAsmX86Instruction *insn = disassemble(); /*throws an exception on error*/
    ASSERT_not_null(insn);

    /* Note successors if necesssary */
    if (successors) {
        bool complete;
        *successors |= insn->getSuccessors(&complete);
    }

    return insn;
}

SgAsmInstruction *
DisassemblerX86::makeUnknownInstruction(const Exception &e)
{
    SgAsmX86Instruction *insn = makeInstruction(x86_unknown_instruction, "unknown");
    insn->set_raw_bytes(e.bytes);
    return insn;
}

/*========================================================================================================================
 * Methods for reading bytes of the instruction.  These keep track of how much has been read, which in turn is used by
 * the makeInstruction method.
 *========================================================================================================================*/

uint8_t
DisassemblerX86::getByte()
{
    if (insnbufat>=15)
        throw ExceptionX86("instruction longer than 15 bytes", this);
    if (insnbufat>=insnbuf.size())
        throw ExceptionX86("short read", this);
    return insnbuf[insnbufat++];
}

uint16_t
DisassemblerX86::getWord()
{
    uint16_t lo = getByte();
    uint16_t hi = getByte();
    return (hi<<8) | lo;
}

uint32_t
DisassemblerX86::getDWord()
{
    uint32_t lo = getWord();
    uint32_t hi = getWord();
    return (hi<<16) | lo;
}

uint64_t
DisassemblerX86::getQWord()
{
    uint64_t lo = getDWord();
    uint64_t hi = getDWord();
    return (hi<<32) | lo;
}




/*========================================================================================================================
 * Miscellaneous helper methods
 *========================================================================================================================*/

SgAsmExpression *
DisassemblerX86::currentDataSegment() const {
    if (segOverride != x86_segreg_none)
        return makeSegmentRegister(segOverride, insnSize==x86_insnsize_64);
    return makeSegmentRegister(x86_segreg_ds, insnSize==x86_insnsize_64);
}

X86InstructionSize
DisassemblerX86::effectiveAddressSize() const
{
    if (addressSizeOverride) {
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
DisassemblerX86::effectiveOperandSize() const
{
    if (operandSizeOverride) {
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
                if (rexPresent && rexW)
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
        if (insnSize == x86_insnsize_64 && !rexW && !sizeMustBe64Bit)
            s = x86_insnsize_32;
        return s;
        /* FIXME: This doesn't handle all of the special cases */
    }
}

DisassemblerX86::MMPrefix
DisassemblerX86::mmPrefix() const
{
    switch (repeatPrefix) {
        case x86_repeat_none: {
            if (operandSizeOverride) {
                return mm66;
            } else {
                return mmNone;
            }
        }
        case x86_repeat_repne: {
            if (operandSizeOverride) {
                throw ExceptionX86("bad combination of repeat prefix and operand size override", this);
            } else {
                return mmF2;
            }
        }
        case x86_repeat_repe: {
            if (operandSizeOverride) {
                throw ExceptionX86("bad combination of repeat prefix and operand size override", this);
            } else {
                return mmF3;
            }
        }
        default: {
            ASSERT_not_reachable("not a valid repeat prefix: " +
                                 stringifyBinaryAnalysisX86RepeatPrefix(repeatPrefix));
            /* avoid MSCV warning by adding return stmt */
            return mmNone;
        }
    }
}

void
DisassemblerX86::setRex(uint8_t opcode)
{
    rexPresent = true;
    rexW = (opcode & 8) != 0;
    rexR = (opcode & 4) != 0;
    rexX = (opcode & 2) != 0;
    rexB = (opcode & 1) != 0;
}

DisassemblerX86::RegisterMode
DisassemblerX86::sizeToMode(X86InstructionSize s)
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
DisassemblerX86::sizeToType(X86InstructionSize s)
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
DisassemblerX86::makeAddrSizeValue(int64_t val, size_t bit_offset, size_t bit_size)
{
    SgAsmValueExpression *retval = NULL;
    switch (effectiveAddressSize()) {
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
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveAddressSize()));
    }
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(bit_size);
    return retval;
}

SgAsmX86Instruction *
DisassemblerX86::makeInstruction(X86InstructionKind kind, const std::string &mnemonic,
                                 SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3, SgAsmExpression *op4)
{
    SgAsmX86Instruction *insn = new SgAsmX86Instruction(ip, mnemonic, kind, insnSize, effectiveOperandSize(),
                                                        effectiveAddressSize());
    ASSERT_not_null(insn);
    insn->set_lockPrefix(lock);
    insn->set_repeatPrefix(repeatPrefix);
    insn->set_raw_bytes(SgUnsignedCharList(&(insnbuf[0]), &(insnbuf[0])+insnbufat));
    if (segOverride != x86_segreg_none)
        insn->set_segmentOverride(segOverride);
    if (branchPredictionEnabled)
        insn->set_branchPrediction(branchPrediction);

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
DisassemblerX86::makeIP()
{
    ASSERT_forbid(REG_IP.isEmpty());
    SgAsmRegisterReferenceExpression *r = new SgAsmDirectRegisterExpression(REG_IP);
    r->set_type(sizeToType(insnSize));
    return r;
}

SgAsmRegisterReferenceExpression *
DisassemblerX86::makeOperandRegisterByte(bool rexExtension, uint8_t registerNumber)
{
    return makeRegister((rexExtension ? 8 : 0) + registerNumber,
                        (rexPresent ? rmRexByte : rmLegacyByte));
}

SgAsmRegisterReferenceExpression *
DisassemblerX86::makeOperandRegisterFull(bool rexExtension, uint8_t registerNumber)
{
    return makeRegister((rexExtension ? 8 : 0) + registerNumber,
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
DisassemblerX86::makeRegister(uint8_t fullRegisterNumber, RegisterMode m, SgAsmType *registerType) const
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
                throw Exception("register number out of bounds");
            if (fullRegisterNumber & 4) {
                name = regnames8h[fullRegisterNumber % 4];
            } else {
                name = regnames8l[fullRegisterNumber % 4];
            }
            registerType = BYTET;
            break;
        case rmRexByte:
            if (fullRegisterNumber >= 16)
                throw Exception("register number out of bounds");
            name = regnames8l[fullRegisterNumber];
            registerType = BYTET;
            break;
        case rmWord:
            if (fullRegisterNumber >= 16)
                throw Exception("register number out of bounds");
            name = regnames16[fullRegisterNumber];
            registerType = WORDT;
            break;
        case rmDWord:
            if (fullRegisterNumber >= 16)
                throw Exception("register number out of bounds");
            name = regnames32[fullRegisterNumber];
            registerType = DWORDT;
            break;
        case rmQWord:
            if (fullRegisterNumber >= 16)
                throw Exception("register number out of bounds");
            name = regnames64[fullRegisterNumber];
            registerType = QWORDT;
            break;
        case rmSegment:
            if (fullRegisterNumber >= 6)
                throw Exception("register number out of bounds");
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
    ASSERT_not_null(registerDictionary());
    const RegisterDescriptor rdesc = registerDictionary()->find(name);
    if (!rdesc)
        throw Exception("register \"" + name + "\" is not available for " + registerDictionary()->get_architecture_name());

    /* Construct the return value. */
    SgAsmRegisterReferenceExpression *rre = NULL;
    if (m != rmST) {
        rre = new SgAsmDirectRegisterExpression(rdesc);
    } else {
        // ST registers are different than most others. Starting with i387, the CPU has eight physical ST registers which
        // are treated as a circular stack, with ST(0) being the top of the stack.  See comments in
        // RegisterDictionary::dictionary_i386_387 for details.
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
DisassemblerX86::makeSegmentRegister(X86SegmentRegister so, bool insn64) const
{
    switch (so) {
        case x86_segreg_none: ASSERT_not_reachable("makeSegmentRegister must not be x86_segreg_none");
        case x86_segreg_cs: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_cs, rmSegment);
        case x86_segreg_ds: return makeRegister(x86_segreg_ds, rmSegment);
        case x86_segreg_es: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_es, rmSegment);
        case x86_segreg_fs: return makeRegister(x86_segreg_fs, rmSegment);
        case x86_segreg_gs: return makeRegister(x86_segreg_gs, rmSegment);
        case x86_segreg_ss: return makeRegister(insn64 ? x86_segreg_ds : x86_segreg_ss, rmSegment);
        default: ASSERT_not_reachable("bad segment register in makeSegmentRegister");
    }

    /* avoid MSCV warning by adding return stmt */
    return NULL;
}





/*========================================================================================================================
 * Methods for operating on the ModR/M byte.
 *========================================================================================================================*/

void
DisassemblerX86::getModRegRM(RegisterMode regMode, RegisterMode rmMode, SgAsmType *t, SgAsmType *tForReg)
{
    if (!tForReg)
        tForReg = t;
    modregrmByte = getByte();
    modregrmByteSet = true;
    modeField = modregrmByte >> 6;
    regField = (modregrmByte & 070) >> 3;
    rmField = modregrmByte & 7;
    reg = makeModrmRegister(regMode, tForReg);
    modrm = makeModrmNormal(rmMode, t);
}

SgAsmMemoryReferenceExpression *
DisassemblerX86::decodeModrmMemory()
{
    ASSERT_require(modregrmByteSet);
    SgAsmExpression* addressExpr = NULL;
    X86SegmentRegister defaultSeg = x86_segreg_ds;
    if (effectiveAddressSize() == x86_insnsize_16) {
        if (modeField == 0 && rmField == 6) {
            /* Special case */
            size_t bit_offset = 8*insnbufat;
            SgAsmValueExpression *ve = SageBuilderAsm::buildValueX86Word(getWord());
            ve->set_bit_offset(bit_offset);
            ve->set_bit_size(32);
            addressExpr = ve;
        } else {
            switch (rmField) {
                case 0:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(3, rmWord), makeRegister(6, rmWord));
                    break;
                case 1:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(3, rmWord), makeRegister(7, rmWord));
                    break;
                case 2:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(5, rmWord), makeRegister(6, rmWord));
                    break;
                case 3:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(5, rmWord), makeRegister(7, rmWord));
                    break;
                case 4:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(6, rmWord);
                    break;
                case 5:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(7, rmWord);
                    break;
                case 6:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = makeRegister(5, rmWord);
                    break;
                case 7:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(3, rmWord);
                    break;
                default: ASSERT_not_reachable("invalid rmField: " + StringUtility::numberToString(rmField));
            }
            switch (modeField) {
                case 0:
                    break; // No offset
                case 1: {
                    size_t bit_offset = 8*insnbufat;
                    uint8_t offset = getByte();
                    SgAsmValueExpression *wv = SageBuilderAsm::buildValueX86Word((int16_t)(int8_t)offset);
                    wv->set_bit_offset(bit_offset);
                    wv->set_bit_size(8);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, wv);
                    break;
                }
                case 2: {
                    size_t bit_offset = 8*insnbufat;
                    uint16_t offset = getWord();
                    SgAsmValueExpression *wv = SageBuilderAsm::buildValueX86Word(offset);
                    wv->set_bit_offset(bit_offset);
                    wv->set_bit_size(16);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, wv);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(modeField));
            }
        }
    } else {
        /* 32 or 64 bits */
        if (modeField == 0 && rmField == 5) {
            /* Special case */
            size_t bit_offset = 8*insnbufat;
            uint32_t offset = getDWord();
            addressExpr = makeAddrSizeValue(IntegerOps::signExtend<32, 64>((uint64_t)offset), bit_offset, 32);
            if (insnSize == x86_insnsize_64) {
                addressExpr = SageBuilderAsm::buildAddExpression(makeIP(), addressExpr);
            }
        } else {
            if (rmField == 4) { /* Need SIB */
                uint8_t sib = getByte();
                uint8_t sibScaleField = sib >> 6;
                uint8_t sibIndexField = (sib & 0070) >> 3;
                uint8_t sibBaseField = sib & 7;
                uint8_t actualScale = (1 << sibScaleField);
                SgAsmExpression* sibBase = NULL;
                if (sibBaseField == 5) {
                    switch (modeField) {
                        case 0: {
                            size_t bit_offset = 8*insnbufat;
                            uint32_t offset = getDWord();
                            sibBase = makeAddrSizeValue(IntegerOps::signExtend<32, 64>((uint64_t)offset), bit_offset, 32);
                            break;
                        }
                        case 1: {
                            sibBase = makeRegister((rexB ? 13 : 5), sizeToMode(insnSize));
                            defaultSeg = x86_segreg_ss;
                            break;
                        }
                        case 2: {
                            sibBase = makeRegister((rexB ? 13 : 5), sizeToMode(insnSize));
                            defaultSeg = x86_segreg_ss;
                            break;
                        }
                        default:
                            ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(modeField));
                    }
                } else {
                    sibBase = makeOperandRegisterFull(rexB, sibBaseField);
                    if (sibBaseField == 4)
                        defaultSeg = x86_segreg_ss;
                }
                if (sibIndexField == 4 && !rexX) {
                    addressExpr = sibBase;
                } else if (actualScale == 1) {
                    addressExpr = SageBuilderAsm::buildAddExpression(sibBase, makeOperandRegisterFull(rexX, sibIndexField));
                } else {
                    SgAsmExpression *regExpr = makeOperandRegisterFull(rexX, sibIndexField);
                    SgAsmExpression *scaleExpr = SageBuilderAsm::buildValueX86Byte(actualScale);
                    SgAsmExpression *productExpr = SageBuilderAsm::buildMultiplyExpression(regExpr, scaleExpr);
                    addressExpr = SageBuilderAsm::buildAddExpression(sibBase, productExpr);
                }
            } else {
                addressExpr = makeOperandRegisterFull(rexB, rmField);
                if (rmField == 5)
                    defaultSeg = x86_segreg_ss;
            }
            switch (modeField) {
                case 0:
                    break; /* No offset */
                case 1: {
                    size_t bit_offset = 8*insnbufat;
                    uint8_t offset = getByte();
                    SgAsmIntegerValueExpression *offsetExpr = SageBuilderAsm::buildValueX86Byte(offset);
                    offsetExpr->set_bit_offset(bit_offset);
                    offsetExpr->set_bit_size(8);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, offsetExpr);
                    break;
                }
                case 2: {
                    size_t bit_offset = 8*insnbufat;
                    uint32_t offset = getDWord();
                    SgAsmIntegerValueExpression *offsetExpr = SageBuilderAsm::buildValueX86DWord(offset);
                    offsetExpr->set_bit_offset(bit_offset);
                    offsetExpr->set_bit_size(32);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, offsetExpr);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(modeField));
            }
        }
    }
    ASSERT_not_null(addressExpr);
    X86SegmentRegister seg;
    if (segOverride != x86_segreg_none) {
        seg = segOverride;
    } else {
        seg = defaultSeg;
    }
    SgAsmMemoryReferenceExpression* mr = SageBuilderAsm::buildMemoryReferenceExpression(addressExpr,
                                                                             makeSegmentRegister(seg, insnSize==x86_insnsize_64));
    return mr;
}

void
DisassemblerX86::fillInModRM(RegisterMode rmMode, SgAsmType *t)
{
    if (modeField == 3) {
        modrm = makeRegister((rexB ? 8 : 0) + rmField, rmMode, t);
    } else {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(t);
    }
}

SgAsmExpression *
DisassemblerX86::makeModrmNormal(RegisterMode m, SgAsmType* mrType)
{
    ASSERT_require(modregrmByteSet);
    if (modeField == 3) {
        /* Register */
        if (m == rmLegacyByte && rexPresent)
            m = rmRexByte;
        return makeRegister((rexB ? 8 : 0) + rmField, m, mrType);
    } else {
        SgAsmMemoryReferenceExpression* modrm = decodeModrmMemory();
        modrm->set_type(mrType);
        return modrm;
    }
}

SgAsmRegisterReferenceExpression *
DisassemblerX86::makeModrmRegister(RegisterMode m, SgAsmType* mrType)
{
    ASSERT_require(modregrmByteSet);
    if (m == rmLegacyByte && rexPresent)
        m = rmRexByte;
    return makeRegister((rexR ? 8 : 0) + regField, m, mrType);
}





/*========================================================================================================================
 * Methods that construct an SgAsmExpression for an immediate operand.
 *========================================================================================================================*/

SgAsmExpression *
DisassemblerX86::getImmByte()
{
    size_t bit_offset = 8*insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86Byte(getByte());
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(8);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmWord()
{
    size_t bit_offset = 8*insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86Word(getWord());
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(16);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmDWord()
{
    size_t bit_offset = 8*insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86DWord(getDWord());
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(32);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmQWord()
{
    size_t bit_offset = 8*insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86QWord(getQWord());
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(64);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmForAddr()
{
    switch (effectiveAddressSize()) {
        case x86_insnsize_16: return getImmWord();
        case x86_insnsize_32: return getImmDWord();
        case x86_insnsize_64: return getImmQWord();
        default: {
            ASSERT_not_reachable("invalid effective address size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveAddressSize()));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
DisassemblerX86::getImmIv()
{
    switch (effectiveOperandSize()) {
        case x86_insnsize_16: return getImmWord();
        case x86_insnsize_32: return getImmDWord();
        case x86_insnsize_64: return getImmQWord();
        default: {
            ASSERT_not_reachable("invalid effective operand size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
DisassemblerX86::getImmJz()
{
    uint64_t val;
    size_t bit_offset=8*insnbufat, bit_size=0;
    if (effectiveOperandSize() == x86_insnsize_16) {
        bit_size = 16;
        uint16_t val2 = getWord();
        val = IntegerOps::signExtend<16, 64>((uint64_t)val2);
    } else {
        bit_size = 32;
        uint32_t val2 = getDWord();
        val = IntegerOps::signExtend<32, 64>((uint64_t)val2);
    }
    uint64_t target = ip + insnbufat + val;
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
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(bit_size);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmByteAsIv()
{
    SgAsmValueExpression *retval = NULL;
    size_t bit_offset = 8*insnbufat;
    uint8_t val = getByte();
    retval = SageBuilderAsm::buildValueX86Byte(val);
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(8);
    return retval;
}

SgAsmExpression *
DisassemblerX86::getImmIzAsIv()
{
    switch (effectiveOperandSize()) {
        case x86_insnsize_16:
            return getImmWord();
        case x86_insnsize_32:
            return getImmDWord();
        case x86_insnsize_64: {
            return getImmDWord();
        }
        default: {
            ASSERT_not_reachable("invalid effective operand size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
DisassemblerX86::getImmJb()
{
    size_t bit_offset = 8*insnbufat;
    uint8_t val = getByte();
    uint64_t target = ip + insnbufat + IntegerOps::signExtend<8, 64>((uint64_t)val);
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
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(8);
    return retval;
}





/*========================================================================================================================
 * Main disassembly functions, each generally containing a huge "switch" statement based on one of the opcode bytes.
 *========================================================================================================================*/

/* Mostly copied from the old x86Disassembler.C version */
SgAsmX86Instruction *
DisassemblerX86::disassemble()
{
    uint8_t opcode = getByte();
    SgAsmX86Instruction *insn = 0;
    switch (opcode) {
        case 0x00: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_add, "add", modrm, reg);
            goto done;
        }
        case 0x01: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_add, "add", modrm, reg);
            goto done;
        }
        case 0x02: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_add, "add", reg, modrm);
            goto done;
        }
        case 0x03: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_add, "add", reg, modrm);
            goto done;
        }
        case 0x04: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_add, "add", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x05: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_add, "add", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x06: {
            not64();
            insn = makeInstruction(x86_push, "push", makeRegister(0, rmSegment));
            goto done;
        }
        case 0x07: {
            not64();
            insn = makeInstruction(x86_pop, "pop", makeRegister(0, rmSegment));
            goto done;
        }
        case 0x08: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_or, "or", modrm, reg);
            goto done;
        }
        case 0x09: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_or, "or", modrm, reg);
            goto done;
        }
        case 0x0A: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_or, "or", reg, modrm);
            goto done;
        }
        case 0x0B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_or, "or", reg, modrm);
            goto done;
        }
        case 0x0C: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_or, "or", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x0D: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_or, "or", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x0E: {
            not64();
            insn = makeInstruction(x86_push, "push", makeRegister(1, rmSegment));
            goto done;
        }
        case 0x0F: {
            insn = decodeOpcode0F();
            goto done;
        }
        case 0x10: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_adc, "adc", modrm, reg);
            goto done;
        }
        case 0x11: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_adc, "adc", modrm, reg);
            goto done;
        }
        case 0x12: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_adc, "adc", reg, modrm);
            goto done;
        }
        case 0x13: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_adc, "adc", reg, modrm);
            goto done;
        }
        case 0x14: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_adc, "adc", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x15: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_adc, "adc", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x16: {
            not64();
            insn = makeInstruction(x86_push, "push", makeRegister(2, rmSegment));
            goto done;
        }
        case 0x17: {
            not64();
            insn = makeInstruction(x86_pop, "pop", makeRegister(2, rmSegment));
            goto done;
        }
        case 0x18: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_sbb, "sbb", modrm, reg);
            goto done;
        }
        case 0x19: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_sbb, "sbb", modrm, reg);
            goto done;
        }
        case 0x1A: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_sbb, "sbb", reg, modrm);
            goto done;
        }
        case 0x1B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_sbb, "sbb", reg, modrm);
            goto done;
        }
        case 0x1C: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_sbb, "sbb", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x1D: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_sbb, "sbb", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x1E: {
            not64();
            insn = makeInstruction(x86_push, "push", makeRegister(3, rmSegment));
            goto done;
        }
        case 0x1F: {
            not64();
            insn = makeInstruction(x86_pop, "pop", makeRegister(3, rmSegment));
            goto done;
        }
        case 0x20: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_and, "and", modrm, reg);
            goto done;
        }
        case 0x21: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_and, "and", modrm, reg);
            goto done;
        }
        case 0x22: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_and, "and", reg, modrm);
            goto done;
        }
        case 0x23: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_and, "and", reg, modrm);
            goto done;
        }
        case 0x24: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_and, "and", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x25: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_and, "and", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x26: {
            segOverride = x86_segreg_es;
            insn = disassemble();
            goto done;
        }
        case 0x27: {
            not64();
            insn = makeInstruction(x86_daa, "daa");
            goto done;
        }
        case 0x28: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_sub, "sub", modrm, reg);
            goto done;
        }
        case 0x29: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_sub, "sub", modrm, reg);
            goto done;
        }
        case 0x2A: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_sub, "sub", reg, modrm);
            goto done;
        }
        case 0x2B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_sub, "sub", reg, modrm);
            goto done;
        }
        case 0x2C: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_sub, "sub", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x2D: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_sub, "sub", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x2E: {
            /* Example of recursive use of disassemble() */
            segOverride = x86_segreg_cs;
            branchPrediction = x86_branch_prediction_not_taken;
            insn = disassemble();
            goto done;
        }
        case 0x2F: {
            not64();
            insn = makeInstruction(x86_das, "das");
            goto done;
        }
        case 0x30: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_xor, "xor", modrm, reg);
            goto done;
        }
        case 0x31: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_xor, "xor", modrm, reg);
            goto done;
        }
        case 0x32: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_xor, "xor", reg, modrm);
            goto done;
        }
        case 0x33: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_xor, "xor", reg, modrm);
            goto done;
        }
        case 0x34: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_xor, "xor", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x35: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_xor, "xor", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x36: {
            segOverride = x86_segreg_ss;
            insn = disassemble();
            goto done;
        }
        case 0x37: {
            not64();
            insn = makeInstruction(x86_aaa, "aaa");
            goto done;
        }
        case 0x38: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_cmp, "cmp", modrm, reg);
            goto done;
        }
        case 0x39: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_cmp, "cmp", modrm, reg);
            goto done;
        }
        case 0x3A: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_cmp, "cmp", reg, modrm);
            goto done;
        }
        case 0x3B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_cmp, "cmp", reg, modrm);
            goto done;
        }
        case 0x3C: {
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_cmp, "cmp", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0x3D: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_cmp, "cmp", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0x3E: {
            segOverride = x86_segreg_ds;
            branchPrediction = x86_branch_prediction_taken;
            insn = disassemble();
            goto done;
        }
        case 0x3F: {
            not64();
            insn = makeInstruction(x86_aas, "aas");
            goto done;
        }
        case 0x40: {
            if (longMode()) {
                setRex(0x40);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(0));
                goto done;
            }
        }
        case 0x41: {
            if (longMode()) {
                setRex(0x41);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(1));
                goto done;
            }
        }
        case 0x42: {
            if (longMode()) {
                setRex(0x42);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(2));
                goto done;
            }
        }
        case 0x43: {
            if (longMode()) {
                setRex(0x43);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(3));
                goto done;
            }
        }
        case 0x44: {
            if (longMode()) {
                setRex(0x44);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(4));
                goto done;
            }
        }
        case 0x45: {
            if (longMode()) {
                setRex(0x45);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(5));
                goto done;
            }
        }
        case 0x46: {
            if (longMode()) {
                setRex(0x46);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(6));
                goto done;
            }
        }
        case 0x47: {
            if (longMode()) {
                setRex(0x47);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_inc, "inc", makeRegisterEffective(7));
                goto done;
            }
        }
        case 0x48: {
            if (longMode()) {
                setRex(0x48);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(0));
                goto done;
            }
        }
        case 0x49: {
            if (longMode()) {
                setRex(0x49);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(1));
                goto done;
            }
        }
        case 0x4A: {
            if (longMode()) {
                setRex(0x4A);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(2));
                goto done;
            }
        }
        case 0x4B: {
            if (longMode()) {
                setRex(0x4B);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(3));
                goto done;
            }
        }
        case 0x4C: {
            if (longMode()) {
                setRex(0x4C);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(4));
                goto done;
            }
        }
        case 0x4D: {
            if (longMode()) {
                setRex(0x4D);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(5));
                goto done;
            }
        }
        case 0x4E: {
            if (longMode()) {
                setRex(0x4E);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(6));
                goto done;
            }
        }
        case 0x4F: {
            if (longMode()) {
                setRex(0x4F);
                insn = disassemble();
                goto done;
            } else {
                insn = makeInstruction(x86_dec, "dec", makeRegisterEffective(7));
                goto done;
            }
        }
        case 0x50: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 0));
            goto done;
        }
        case 0x51: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 1));
            goto done;
        }
        case 0x52: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 2));
            goto done;
        }
        case 0x53: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 3));
            goto done;
        }
        case 0x54: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 4));
            goto done;
        }
        case 0x55: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 5));
            goto done;
        }
        case 0x56: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 6));
            goto done;
        }
        case 0x57: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_push, "push", makeRegisterEffective(rexB, 7));
            goto done;
        }
        case 0x58: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 0));
            goto done;
        }
        case 0x59: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 1));
            goto done;
        }
        case 0x5A: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 2));
            goto done;
        }
        case 0x5B: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 3));
            goto done;
        }
        case 0x5C: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 4));
            goto done;
        }
        case 0x5D: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 5));
            goto done;
        }
        case 0x5E: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 6));
            goto done;
        }
        case 0x5F: {
            sizeMustBe64Bit = true;
            insn = makeInstruction(x86_pop, "pop", makeRegisterEffective(rexB, 7));
            goto done;
        }
        case 0x60: {
            not64();
            if (effectiveOperandSize() == x86_insnsize_32) {
                insn = makeInstruction(x86_pushad, "pushad");
            } else {
                insn = makeInstruction(x86_pusha, "pusha");
            }
            goto done;
        }
        case 0x61: {
            not64();
            if (effectiveOperandSize() == x86_insnsize_32) {
                insn = makeInstruction(x86_popad, "popad");
            } else {
                insn = makeInstruction(x86_popa, "popa");
            }
            goto done;
        }
        case 0x62: {
            not64();
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            insn = makeInstruction(x86_bound, "bound", reg, modrm);
            goto done;
        }
        case 0x63: {
            if (longMode()) {
                getModRegRM(effectiveOperandMode(), rmDWord, DWORDT);
                insn = makeInstruction(x86_movsxd, "movsxd", reg, modrm);
                goto done;
            } else {
                getModRegRM(rmWord, rmWord, WORDT);
                insn = makeInstruction(x86_arpl, "arpl", modrm, reg);
                goto done;
            }
        }
        case 0x64: {
            segOverride = x86_segreg_fs;
            insn = disassemble();
            goto done;
        }
        case 0x65: {
            segOverride = x86_segreg_gs;
            insn = disassemble();
            goto done;
        }
        case 0x66: {
            operandSizeOverride = true;
            insn = disassemble();
            goto done;
        }
        case 0x67: {
            addressSizeOverride = true;
            insn = disassemble();
            goto done;
        }
        case 0x68: {
            sizeMustBe64Bit = true;
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_push, "push", imm);
            goto done;
        }
        case 0x69: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_imul, "imul", reg, modrm, imm);
            goto done;
        }
        case 0x6A: {
            sizeMustBe64Bit = true;
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_push, "push", imm);
            goto done;
        }
        case 0x6B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmByteAsIv();
            insn = makeInstruction(x86_imul, "imul", reg, modrm, imm);
            goto done;
        }
        case 0x6C: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_insb, "insb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_rep_insb, "rep_insb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for insb", this);
            }
        }
        case 0x6D: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_insw, "insw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_insw, "rep_insw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for insw", this);
                    }
                case x86_insnsize_32:
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_insd, "insd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_insd, "rep_insd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for insd", this);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x6E: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_outsb, "outsb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_rep_outsb, "rep_outsb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for outsb", this);
            }
        }
        case 0x6F: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_outsw, "outsw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_outsw, "rep_outsw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for outsw", this);
                    }
                case x86_insnsize_32:
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_outsd, "outsd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_outsd, "rep_outsd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for outsd", this);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x70: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jo, "jo", imm);
            goto done;
        }
        case 0x71: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jno, "jno", imm);
            goto done;
        }
        case 0x72: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jb, "jb", imm);
            goto done;
        }
        case 0x73: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jae, "jae", imm);
            goto done;
        }
        case 0x74: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_je, "je", imm);
            goto done;
        }
        case 0x75: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jne, "jne", imm);
            goto done;
        }
        case 0x76: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jbe, "jbe", imm);
            goto done;
        }
        case 0x77: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_ja, "ja", imm);
            goto done;
        }
        case 0x78: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_js, "js", imm);
            goto done;
        }
        case 0x79: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jns, "jns", imm);
            goto done;
        }
        case 0x7A: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jpe, "jpe", imm);
            goto done;
        }
        case 0x7B: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jpo, "jpo", imm);
            goto done;
        }
        case 0x7C: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jl, "jl", imm);
            goto done;
        }
        case 0x7D: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jge, "jge", imm);
            goto done;
        }
        case 0x7E: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jle, "jle", imm);
            goto done;
        }
        case 0x7F: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            insn = makeInstruction(x86_jg, "jg", imm);
            goto done;
        }
        case 0x80: {
            /* The names for groups will make more sense relative to the AMD manual. */
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte();
            insn = decodeGroup1(imm);
            goto done;
        }
        case 0x81: {
            /* effectiveOperandMode() returns register mode for the effective operand size (16bit, 32, bit, 64bit)
             * effectiveOperandType() does the same thing but returne a SgAsmType. */
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmIzAsIv();
            insn = decodeGroup1(imm);
            goto done;
        }
        case 0x82: {
            not64();
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte();
            insn = decodeGroup1(imm);
            goto done;
        }
        case 0x83: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmByteAsIv();
            insn = decodeGroup1(imm);
            goto done;
        }
        case 0x84: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_test, "test", modrm, reg);
            goto done;
        }
        case 0x85: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_test, "test", modrm, reg);
            goto done;
        }
        case 0x86: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_xchg, "xchg", modrm, reg);
            goto done;
        }
        case 0x87: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_xchg, "xchg", modrm, reg);
            goto done;
        }
        case 0x88: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_mov, "mov", modrm, reg);
            goto done;
        }
        case 0x89: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_mov, "mov", modrm, reg);
            goto done;
        }
        case 0x8A: {
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(x86_mov, "mov", reg, modrm);
            goto done;
        }
        case 0x8B: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            insn = makeInstruction(x86_mov, "mov", reg, modrm);
            goto done;
        }
        case 0x8C: {
            getModRegRM(rmSegment, effectiveOperandMode(), WORDT);
            insn = makeInstruction(x86_mov, "mov", modrm, reg);
            goto done;
        }
        case 0x8D: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            insn = makeInstruction(x86_lea, "lea", reg, modrm);
            goto done;
        }
        case 0x8E: {
            getModRegRM(rmSegment, rmWord, WORDT);
            insn = makeInstruction(x86_mov, "mov", reg, modrm);
            goto done;
        }
        case 0x8F: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            insn = decodeGroup1a();
            goto done;
        }
        case 0x90: {
            if (rexB) {
                insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(8), makeRegisterEffective(0));
                goto done;
            } else if (repeatPrefix == x86_repeat_repe) {
                insn = makeInstruction(x86_pause, "pause");
                goto done;
            } else {
                insn = makeInstruction(x86_nop, "nop");
                goto done;
            }
        }
        case 0x91: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 1), makeRegisterEffective(0));
            goto done;
        }
        case 0x92: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 2), makeRegisterEffective(0));
            goto done;
        }
        case 0x93: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 3), makeRegisterEffective(0));
            goto done;
        }
        case 0x94: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 4), makeRegisterEffective(0));
            goto done;
        }
        case 0x95: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 5), makeRegisterEffective(0));
            goto done;
        }
        case 0x96: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 6), makeRegisterEffective(0));
            goto done;
        }
        case 0x97: {
            insn = makeInstruction(x86_xchg, "xchg", makeRegisterEffective(rexB, 7), makeRegisterEffective(0));
            goto done;
        }
        case 0x98: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    insn = makeInstruction(x86_cbw, "cbw");
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(x86_cwde, "cwde");
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(x86_cdqe, "cdqe");
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x99: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    insn = makeInstruction(x86_cwd, "cwd");
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(x86_cdq, "cdq");
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(x86_cqo, "cqo");
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x9A: {
            not64();
            SgAsmExpression* addr = getImmForAddr();
            SgAsmExpression* seg = getImmWord();
            insn = makeInstruction(x86_farcall, "farCall", seg, addr);
            goto done;
        }
        case 0x9B: {
            insn = makeInstruction(x86_wait, "wait");
            goto done;
        }
        case 0x9C: {
            sizeMustBe64Bit = true;
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    insn = makeInstruction(x86_pushf, "pushf");
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(x86_pushfd, "pushfd");
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(x86_pushfq, "pushfq");
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x9D: {
            sizeMustBe64Bit = true;
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    insn = makeInstruction(x86_popf, "popf");
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(x86_popfd, "popfd");
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(x86_popfq, "popfq");
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0x9E: {
            insn = makeInstruction(x86_sahf, "sahf");
            goto done;
        }
        case 0x9F: {
            insn = makeInstruction(x86_lahf, "lahf");
            goto done;
        }
        case 0xA0: {
            SgAsmExpression* addr = getImmForAddr();
            insn = makeInstruction(x86_mov, "mov", makeRegister(0, rmLegacyByte),
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(), BYTET));
            goto done;
        }
        case 0xA1: {
            SgAsmExpression* addr = getImmForAddr();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(0),
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(), effectiveOperandType()));
            goto done;
        }
        case 0xA2: {
            SgAsmExpression* addr = getImmForAddr();
            insn = makeInstruction(x86_mov, "mov",
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(), BYTET),
                                         makeRegister(0, rmLegacyByte));
            goto done;
        }
        case 0xA3: {
            SgAsmExpression* addr = getImmForAddr();
            insn = makeInstruction(x86_mov, "mov",
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(), effectiveOperandType()),
                                         makeRegisterEffective(0));
            goto done;
        }
        case 0xA4: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_movsb, "movsb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_rep_movsb, "rep_movsb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for movsb", this);
            }
        }
        case 0xA5: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_movsw, "movsw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_movsw, "rep_movsw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsw", this);
                    }
                case x86_insnsize_32:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_movsd, "movsd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_movsd, "rep_movsd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsd", this);
                    }
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_movsq, "movsq");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_movsq, "rep_movsq");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsq", this);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xA6: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_cmpsb, "cmpsb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_repe_cmpsb, "repe_cmpsb");
                    goto done;
                case x86_repeat_repne:
                    insn = makeInstruction(x86_repne_cmpsb, "repne_cmpsb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for cmpsb", this);
            }
        }
        case 0xA7: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_cmpsw, "cmpsw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_cmpsw, "repe_cmpsw");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_cmpsw, "repne_cmpsw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsw", this);
                    }
                case x86_insnsize_32:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_cmpsd, "cmpsd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_cmpsd, "repe_cmpsd");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_cmpsd, "repne_cmpsd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsd", this);
                    }
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_cmpsq, "cmpsq");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_cmpsq, "repe_cmpsq");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_cmpsq, "repne_cmpsq");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsq", this);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xA8: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_test, "test", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0xA9: {
            SgAsmExpression* imm = getImmIzAsIv();
            insn = makeInstruction(x86_test, "test", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0xAA: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_stosb, "stosb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_rep_stosb, "rep_stosb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for stosb", this);
            }
        }
        case 0xAB: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_stosw, "stosw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_stosw, "rep_stosw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for stosw", this);
                    }
                case x86_insnsize_32:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_stosd, "stosd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_stosd, "rep_stosd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for stosd", this);
                    }
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_stosq, "stosq");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_stosq, "rep_stosq");
                            goto done;
                        default: throw ExceptionX86("bad repeat prefix for stosq", this);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xAC: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_lodsb, "lodsb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_rep_lodsb, "rep_lodsb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for lodsb", this);
            }
        }
        case 0xAD: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_lodsw, "lodsw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_lodsw, "rep_lodsw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsw", this);
                    }
                case x86_insnsize_32:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_lodsd, "lodsd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_lodsd, "rep_lodsd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsd", this);
                    }
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_lodsq, "lodsq");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_rep_lodsq, "rep_lodsq");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsq", this);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xAE: {
            switch (repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(x86_scasb, "scasb");
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(x86_repe_scasb, "repe_scasb");
                    goto done;
                case x86_repeat_repne:
                    insn = makeInstruction(x86_repne_scasb, "repne_scasb");
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for scasb", this);
            }
        }
        case 0xAF: {
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_scasw, "scasw");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_scasw, "repe_scasw");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_scasw, "repne_scasw");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasw", this);
                    }
                case x86_insnsize_32:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_scasd, "scasd");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_scasd, "repe_scasd");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_scasd, "repne_scasd");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasd", this);
                    }
                case x86_insnsize_64:
                    switch (repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(x86_scasq, "scasq");
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(x86_repe_scasq, "repe_scasq");
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(x86_repne_scasq, "repne_scasq");
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasq", this);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xB0: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 0), imm);
            goto done;
        }
        case 0xB1: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 1), imm);
            goto done;
        }
        case 0xB2: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 2), imm);
            goto done;
        }
        case 0xB3: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 3), imm);
            goto done;
        }
        case 0xB4: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 4), imm);
            goto done;
        }
        case 0xB5: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 5), imm);
            goto done;
        }
        case 0xB6: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 6), imm);
            goto done;
        }
        case 0xB7: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_mov, "mov", makeOperandRegisterByte(rexB, 7), imm);
            goto done;
        }
        case 0xB8: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 0), imm);
            goto done;
        }
        case 0xB9: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 1), imm);
            goto done;
        }
        case 0xBA: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 2), imm);
            goto done;
        }
        case 0xBB: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 3), imm);
            goto done;
        }
        case 0xBC: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 4), imm);
            goto done;
        }
        case 0xBD: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 5), imm);
            goto done;
        }
        case 0xBE: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 6), imm);
            goto done;
        }
        case 0xBF: {
            SgAsmExpression* imm = getImmIv();
            insn = makeInstruction(x86_mov, "mov", makeRegisterEffective(rexB, 7), imm);
            goto done;
        }
        case 0xC0: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte();
            insn = decodeGroup2(imm);
            goto done;
        }
        case 0xC1: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmByteAsIv();
            insn = decodeGroup2(imm);
            goto done;
        }
        case 0xC2: {
            isUnconditionalJump = true;
            SgAsmExpression* imm = getImmWord();
            insn = makeInstruction(x86_ret, "ret", imm);
            goto done;
        }
        case 0xC3: {
            isUnconditionalJump = true;
            insn = makeInstruction(x86_ret, "ret");
            goto done;
        }
        case 0xC4: {
            not64();
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            insn = makeInstruction(x86_les, "les", reg, modrm);
            goto done;
        }
        case 0xC5: {
            not64();
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            insn = makeInstruction(x86_lds, "lds", reg, modrm);
            goto done;
        }
        case 0xC6: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte();
            insn = decodeGroup11(imm);
            goto done;
        }
        case 0xC7: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* imm = getImmIzAsIv();
            insn = decodeGroup11(imm);
            goto done;
        }
        case 0xC8: {
            SgAsmExpression* immw = getImmWord();
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_enter, "enter", immw, imm);
            goto done;
        }
        case 0xC9: {
            insn = makeInstruction(x86_leave, "leave");
            goto done;
        }
        case 0xCA: {
            isUnconditionalJump = true;
            SgAsmExpression* imm = getImmWord();
            insn = makeInstruction(x86_retf, "retf", imm);
            goto done;
        }
        case 0xCB: {
            isUnconditionalJump = true;
            insn = makeInstruction(x86_retf, "retf");
            goto done;
        }
        case 0xCC: {
            insn = makeInstruction(x86_int3, "int3");
            goto done;
        }
        case 0xCD: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_int, "int", imm);
            goto done;
        }
        case 0xCE: {
            not64();
            insn = makeInstruction(x86_into, "into");
            goto done;
        }
        case 0xCF: {
            isUnconditionalJump = true;
            insn = makeInstruction(x86_iret, "iret");
            goto done;
        }
        case 0xD0: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup2(SageBuilderAsm::buildValueX86Byte(1));
            goto done;
        }
        case 0xD1: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            insn = decodeGroup2(SageBuilderAsm::buildValueX86Byte(1));
            goto done;
        }
        case 0xD2: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup2(makeRegister(1, rmLegacyByte));
            goto done;
        }
        case 0xD3: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            insn = decodeGroup2(makeRegister(1, rmLegacyByte));
            goto done;
        }
        case 0xD4: {
            not64();
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_aam, "aam", imm);
            goto done;
        }
        case 0xD5: {
            not64();
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_aad, "aad", imm);
            goto done;
        }
        case 0xD6: {
            not64();
            insn = makeInstruction(x86_salc, "salc");
            goto done;
        }
        case 0xD7: {
            insn = makeInstruction(x86_xlatb, "xlatb");
            goto done;
        }
        case 0xD8: {
            insn = decodeX87InstructionD8();
            goto done;
        }
        case 0xD9: {
            insn = decodeX87InstructionD9();
            goto done;
        }
        case 0xDA: {
            insn = decodeX87InstructionDA();
            goto done;
        }
        case 0xDB: {
            insn = decodeX87InstructionDB();
            goto done;
        }
        case 0xDC: {
            insn = decodeX87InstructionDC();
            goto done;
        }
        case 0xDD: {
            insn = decodeX87InstructionDD();
            goto done;
        }
        case 0xDE: {
            insn = decodeX87InstructionDE();
            goto done;
        }
        case 0xDF: {
            insn = decodeX87InstructionDF();
            goto done;
        }
        case 0xE0: {
            SgAsmExpression* imm = getImmJb();
            insn = makeInstruction(x86_loopnz, "loopnz", imm);
            goto done;
        }
        case 0xE1: {
            SgAsmExpression* imm = getImmJb();
            insn = makeInstruction(x86_loopz, "loopz", imm);
            goto done;
        }
        case 0xE2: {
            SgAsmExpression* imm = getImmJb();
            insn = makeInstruction(x86_loop, "loop", imm);
            goto done;
        }
        case 0xE3: {
            SgAsmExpression* imm = getImmJb();
            branchPredictionEnabled = true;
            switch (effectiveOperandSize()) {
                case x86_insnsize_16:
                    insn = makeInstruction(x86_jcxz, "jcxz", imm);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(x86_jecxz, "jecxz", imm);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(x86_jrcxz, "jrcxz", imm);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize()));
            }
        }
        case 0xE4: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_in, "in", makeRegister(0, rmLegacyByte), imm);
            goto done;
        }
        case 0xE5: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_in, "in", makeRegisterEffective(0), imm);
            goto done;
        }
        case 0xE6: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_out, "out", imm, makeRegister(0, rmLegacyByte));
            goto done;
        }
        case 0xE7: {
            SgAsmExpression* imm = getImmByte();
            insn = makeInstruction(x86_out, "out", imm, makeRegisterEffective(0));
            goto done;
        }
        case 0xE8: {
            SgAsmExpression* imm = getImmJz();
            insn = makeInstruction(x86_call, "call", imm);
            goto done;
        }
        case 0xE9: {
            SgAsmExpression* imm = getImmJz();
            insn = makeInstruction(x86_jmp, "jmp", imm);
            isUnconditionalJump = true;
            goto done;
        }
        case 0xEA: {
            not64();
            SgAsmExpression* addr = getImmForAddr();
            SgAsmExpression* seg = getImmWord();
            insn = makeInstruction(x86_farjmp, "farJmp", seg, addr);
            isUnconditionalJump = true;
            goto done;
        }
        case 0xEB: {
            SgAsmExpression* imm = getImmJb();
            insn = makeInstruction(x86_jmp, "jmp", imm);
            isUnconditionalJump = true;
            goto done;
        }
        case 0xEC: {
            insn = makeInstruction(x86_in, "in", makeRegister(0, rmLegacyByte), makeRegister(2, rmWord));
            goto done;
        }
        case 0xED: {
            insn = makeInstruction(x86_in, "in", makeRegisterEffective(0), makeRegister(2, rmWord));
            goto done;
        }
        case 0xEE: {
            insn = makeInstruction(x86_out, "out", makeRegister(2, rmWord), makeRegister(0, rmLegacyByte));
            goto done;
        }
        case 0xEF: {
            insn = makeInstruction(x86_out, "out", makeRegister(2, rmWord), makeRegisterEffective(0));
            goto done;
        }
        case 0xF0: {
            lock = true;
            insn = disassemble();
            goto done;
        }
        case 0xF1: {
            insn = makeInstruction(x86_int1, "int1");
            goto done;
        }
        case 0xF2: {
            repeatPrefix = x86_repeat_repne;
            insn = disassemble();
            goto done;
        }
        case 0xF3: {
            repeatPrefix = x86_repeat_repe;
            insn = disassemble();
            goto done;
        }
        case 0xF4: {
            insn = makeInstruction(x86_hlt, "hlt");
            isUnconditionalJump = true;
            goto done;
        }
        case 0xF5: {
            insn = makeInstruction(x86_cmc, "cmc");
            goto done;
        }
        case 0xF6: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* immMaybe = NULL;
            if (regField <= 1) {
                SgAsmExpression* imm = getImmByteAsIv();
                immMaybe = imm;
            }
            insn = decodeGroup3(immMaybe);
            goto done;
        }
        case 0xF7: {
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            SgAsmExpression* immMaybe = NULL;
            if (regField <= 1) {
                SgAsmExpression* imm = getImmIzAsIv();
                immMaybe = imm;
            }
            insn = decodeGroup3(immMaybe);
            goto done;
        }
        case 0xF8: {
            insn = makeInstruction(x86_clc, "clc");
            goto done;
        }
        case 0xF9: {
            insn = makeInstruction(x86_stc, "stc");
            goto done;
        }
        case 0xFA: {
            insn = makeInstruction(x86_cli, "cli");
            goto done;
        }
        case 0xFB: {
            insn = makeInstruction(x86_sti, "sti");
            goto done;
        }
        case 0xFC: {
            insn = makeInstruction(x86_cld, "cld");
            goto done;
        }
        case 0xFD: {
            insn = makeInstruction(x86_std, "std");
            goto done;
        }
        case 0xFE: {
            getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup4();
            goto done;
        }
        case 0xFF: {
            getModRegRM(rmReturnNull, rmReturnNull, NULL);
            if (regField >= 2 && regField <= 6) {
                sizeMustBe64Bit = true;
            }
            fillInModRM(effectiveOperandMode(), effectiveOperandType());
            insn = decodeGroup5();
            goto done;
        }
        default: ASSERT_not_reachable("should not get here");
    }
done:
    ASSERT_not_null(insn);
    return insn;
}

SgAsmX86Instruction *
DisassemblerX86::decodeOpcode0F()
{
    uint8_t opcode = getByte();
    switch (opcode) {
        case 0x00: {
            getModRegRM(rmReturnNull, rmWord, WORDT);
            return decodeGroup6();
        }
        case 0x01:
            return decodeGroup7();
        case 0x02: {
            getModRegRM(rmWord, rmWord, WORDT);
            return makeInstruction(x86_lar, "lar", reg, modrm);
        }
        case 0x03: {
            getModRegRM(rmWord, rmWord, WORDT);
            return makeInstruction(x86_lsl, "lsl", reg, modrm);
        }
        case 0x04:
            throw ExceptionX86("bad opcode 0x0f04", this);
        case 0x05:
            return makeInstruction(x86_syscall, "syscall");
        case 0x06:
            return makeInstruction(x86_clts, "clts");
        case 0x07:
            return makeInstruction(x86_sysret, "sysret");
        case 0x08:
            return makeInstruction(x86_invd, "invd");
        case 0x09:
            return makeInstruction(x86_wbinvd, "wbinvd");
        case 0x0A:
            throw ExceptionX86("bad opcode 0x0f0a", this);
        case 0x0B:
            return makeInstruction(x86_ud2, "ud2");
        case 0x0C:
            throw ExceptionX86("bad opcode 0x0f0c", this);
        case 0x0D:
            return decodeGroupP();
        case 0x0E:
            return makeInstruction(x86_femms, "femms");
        case 0x0F: {
            /* 3DNow! (AMD Specific) */
            getModRegRM(rmReturnNull, rmReturnNull, NULL);
            uint8_t thirdOpcodeByte = getByte();
            char opcodestr[16];
            sprintf(opcodestr, "0x0f0f%02x", thirdOpcodeByte);
            if (thirdOpcodeByte < 0x80) {
                /* Conversions */
                switch (thirdOpcodeByte) {
                    case 0x0C: {
                        fillInModRM(rmMM, V4WORDT);
                        reg = makeModrmRegister(rmMM, V2FLOATT);
                        return makeInstruction(x86_pi2fw, "pi2fw", reg, modrm);
                    }
                    case 0x0D: {
                        fillInModRM(rmMM, V2DWORDT);
                        reg = makeModrmRegister(rmMM, V2FLOATT);
                        return makeInstruction(x86_pi2fd, "pi2fd", reg, modrm);
                    }
                    case 0x1C: {
                        fillInModRM(rmMM, V2FLOATT);
                        reg = makeModrmRegister(rmMM, V4WORDT);
                        return makeInstruction(x86_pf2iw, "pf2iw", reg, modrm);
                    }
                    case 0x1D: {
                        fillInModRM(rmMM, V2FLOATT);
                        reg = makeModrmRegister(rmMM, V2DWORDT);
                        return makeInstruction(x86_pf2id, "pf2id", reg, modrm);
                    }
                    default:
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, this);
                }
            } else if (thirdOpcodeByte < 0xB7) {
                /* Floating-point operations */
                fillInModRM(rmMM, V2FLOATT);
                reg = makeModrmRegister(rmMM, V2FLOATT);
                switch (thirdOpcodeByte) {
                    case 0x8A: return makeInstruction(x86_pfnacc, "pfnacc", reg, modrm);
                    case 0x8E: return makeInstruction(x86_pfpnacc, "pfpnacc", reg, modrm);
                    case 0x90: return makeInstruction(x86_pfcmpge, "pfcmpge", reg, modrm);
                    case 0x94: return makeInstruction(x86_pfmin, "pfmin", reg, modrm);
                    case 0x96: return makeInstruction(x86_pfrcp, "pfrcp", reg, modrm);
                    case 0x97: return makeInstruction(x86_pfrsqrt, "pfrsqrt", reg, modrm);
                    case 0x9A: return makeInstruction(x86_pfsub, "pfsub", reg, modrm);
                    case 0x9E: return makeInstruction(x86_pfadd, "pfadd", reg, modrm);
                    case 0xA0: return makeInstruction(x86_pfcmpgt, "pfcmpgt", reg, modrm);
                    case 0xA4: return makeInstruction(x86_pfmax, "pfmax", reg, modrm);
                    case 0xA6: return makeInstruction(x86_pfrcpit1, "pfrcpit1", reg, modrm);
                    case 0xA7: return makeInstruction(x86_pfrsqit1, "pfrsqit1", reg, modrm);
                    case 0xAA: return makeInstruction(x86_pfsubr, "pfsubr", reg, modrm);
                    case 0xAE: return makeInstruction(x86_pfacc, "pfacc", reg, modrm);
                    case 0xB0: return makeInstruction(x86_pfcmpeq, "pfcmpeq", reg, modrm);
                    case 0xB4: return makeInstruction(x86_pfmul, "pfmul", reg, modrm);
                    case 0xB6: return makeInstruction(x86_pfrcpit2, "pfrcpit2", reg, modrm);
                    default: {
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, this);
                    }
                }
            } else {
                /* Extra integer operations */
                switch (thirdOpcodeByte) {
                    case 0xB7:
                        getModRegRM(rmMM, rmMM, V4WORDT);
                        return makeInstruction(x86_pmulhrw, "pmulhrw", reg, modrm);
                    case 0xBB:
                        getModRegRM(rmMM, rmMM, V2DWORDT);
                        return makeInstruction(x86_pswapd, "pswapd", reg, modrm);
                    case 0xBF:
                        getModRegRM(rmMM, rmMM, V8BYTET);
                        return makeInstruction(x86_pavgusb, "pavgusb", reg, modrm);
                    default:
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, this);
                }
            }
        }
        case 0x10: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movups, "movups", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_movss, "movss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_movupd, "movupd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_movsd_sse, "movsd", reg, modrm);
            }
        }
        case 0x11: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movups, "movups", modrm, reg);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_movss, "movss", modrm, reg);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_movupd, "movupd", modrm, reg);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_movsd_sse, "movsd", modrm, reg);
            }
        }
        case 0x12: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movhlps, "movhlps", reg, modrm);
                    } else {
                        return makeInstruction(x86_movlps, "movlps", reg, modrm);
                    }
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movsldup, "movsldup", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    requireMemory();
                    return makeInstruction(x86_movlpd, "movlpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_movddup, "movddup", reg, modrm);
            }
        }
        case 0x13: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    requireMemory();
                    return makeInstruction(x86_movlps, "movlps", modrm, reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f13", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    requireMemory();
                    return makeInstruction(x86_movlpd, "movlpd", modrm, reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f13", this);
            }
        }
        case 0x14: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V2FLOATT, V4FLOATT);
                    return makeInstruction(x86_unpcklps, "unpcklps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f14", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET, V2DOUBLET);
                    return makeInstruction(x86_unpcklpd, "unpcklpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f14", this);
            }
        }
        case 0x15: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V2FLOATT, V4FLOATT);
                    return makeInstruction(x86_unpckhps, "unpckhps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f15", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET, V2DOUBLET);
                    return makeInstruction(x86_unpckhpd, "unpckhpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f15", this);
            }
        }
        case 0x16: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movlhps, "movlhps", reg, modrm);
                    } else {
                        return makeInstruction(x86_movhps, "movhps", reg, modrm);
                    }
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movshdup, "movshdup", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    requireMemory();
                    return makeInstruction(x86_movhpd, "movhpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f16", this);
            }
        }
        case 0x17: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    requireMemory();
                    return makeInstruction(x86_movhps, "movhps", modrm, reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f17", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    requireMemory();
                    return makeInstruction(x86_movhpd, "movhpd", modrm, reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f17", this);
            }
        }
        case 0x18:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return decodeGroup16();
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            /* Undocumented no-ops */
            getModRegRM(rmReturnNull, rmReturnNull, NULL);
            return makeInstruction(x86_nop, "nop");
        case 0x1F:
            /* Documented no-op */
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_nop, "nop", modrm);

        case 0x20:
            /* BUG: The mode and type fields should forced to the current processor number of bits instead of the size
             *      determied by the operand size flag. See documentation for move  to control register ("lock mov cr0, *").
             *      This may be an AMD specific issue, but the  operand size issues is a bug everywhere. */
            getModRegRM(rmControl, effectiveOperandMode(), effectiveOperandType());
            if (modeField == 3) {
                return makeInstruction(x86_mov, "mov", modrm, reg);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f20", this);
            }
        case 0x21:
            getModRegRM(rmDebug, effectiveOperandMode(), effectiveOperandType());
            if (modeField == 3) {
                return makeInstruction(x86_mov, "mov", modrm, reg);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f21", this);
            }
        case 0x22:
            getModRegRM(rmControl, effectiveOperandMode(), effectiveOperandType());
            if (modeField == 3) {
                return makeInstruction(x86_mov, "mov", reg, modrm);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f22", this);
            }
        case 0x23:
            getModRegRM(rmDebug, effectiveOperandMode(), effectiveOperandType());
            if (modeField == 3) {
                return makeInstruction(x86_mov, "mov", reg, modrm);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f23", this);
            }
        case 0x24:
            /* Move from test register */
            throw ExceptionX86("bad opcode 0x0f24", this);
        case 0x25:
            throw ExceptionX86("bad opcode 0x0f25", this);
        case 0x26:
            /* Move to test register */
            throw ExceptionX86("bad opcode 0x0f26", this);
        case 0x27:
            throw ExceptionX86("bad opcode 0x0f27", this);
        case 0x28: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movaps, "movaps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f28", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_movapd, "movapd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f18", this);
            }
        }
        case 0x29: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movaps, "movaps", modrm, reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f29", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_movapd, "movapd", modrm, reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f19", this);
            }
        }
        case 0x2A: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmMM, V2DWORDT, V4FLOATT);
                    return makeInstruction(x86_cvtpi2ps, "cvtpi2ps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V4FLOATT);
                    return makeInstruction(x86_cvtsi2ss, "cvtsi2ss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmMM, V2DWORDT, V2DOUBLET);
                    return makeInstruction(x86_cvtpi2pd, "cvtpi2pd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V2DOUBLET);
                    return makeInstruction(x86_cvtsi2sd, "cvtsi2sd", reg, modrm);
            }
        }
        case 0x2B: {
            requireMemory();
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_movntps, "movntps", modrm, reg);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_movntss, "movntss", modrm, reg);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_movntpd, "movntpd", modrm, reg);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_movntsd, "movntsd", modrm, reg);
            }
        }
        case 0x2C: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmXMM, V4FLOATT, V2DWORDT);
                    return makeInstruction(x86_cvttps2pi, "cvttps2pi", reg, modrm);
                case mmF3:
                    getModRegRM(effectiveOperandMode(), rmXMM, FLOATT, effectiveOperandType());
                    return makeInstruction(x86_cvttss2si, "cvttss2si", reg, modrm);
                case mm66:
                    getModRegRM(rmMM, rmXMM, V2DOUBLET, V2DWORDT);
                    return makeInstruction(x86_cvttpd2pi, "cvttpd2pi", reg, modrm);
                case mmF2:
                    getModRegRM(effectiveOperandMode(), rmXMM, DOUBLET, effectiveOperandType());
                    return makeInstruction(x86_cvttsd2si, "cvttsd2si", reg, modrm);
            }
        }
        case 0x2D: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmXMM, V4FLOATT, V2DWORDT);
                    return makeInstruction(x86_cvtps2pi, "cvtps2pi", reg, modrm);
                case mmF3:
                    getModRegRM(effectiveOperandMode(), rmXMM, V4FLOATT, effectiveOperandType());
                    return makeInstruction(x86_cvtss2si, "cvtss2si", reg, modrm);
                case mm66:
                    getModRegRM(rmMM, rmXMM, V2DOUBLET, V2DWORDT);
                    return makeInstruction(x86_cvtpd2pi, "cvtpd2pi", reg, modrm);
                case mmF2:
                    getModRegRM(effectiveOperandMode(), rmXMM, V2DOUBLET, effectiveOperandType());
                    return makeInstruction(x86_cvtsd2si, "cvtsd2si", reg, modrm);
            }
        }
        case 0x2E: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_ucomiss, "ucomiss", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f2e", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_ucomisd, "ucomisd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f2e", this);
            }
        }
        case 0x2F: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_comiss, "comiss", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f2f", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_comisd, "comisd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f2f", this);
            }
        }
        case 0x30:
            return makeInstruction(x86_wrmsr, "wrmsr");
        case 0x31:
            return makeInstruction(x86_rdtsc, "rdtsc");
        case 0x32:
            return makeInstruction(x86_rdmsr, "rdmsr");
        case 0x33:
            return makeInstruction(x86_rdpmc, "rdpmc");
        case 0x34:
            not64();
            return makeInstruction(x86_sysenter, "sysenter");
        case 0x35:
            not64();
            return makeInstruction(x86_sysexit, "sysexit");
        case 0x36:
            throw ExceptionX86("bad opcode 0x0f36", this);
        case 0x37:
            return makeInstruction(x86_getsec, "getsec");
        case 0x38:
            decodeOpcode0F38(); /*SSSE3*/
        case 0x39:
            throw ExceptionX86("bad opcode 0x0f39", this);
        case 0x3A: {
            /* more SSE3? should this be in a decodeOpcode0F3A() instead? */
            uint8_t thirdOpcodeByte = getByte();
            switch (thirdOpcodeByte) {
                case 0x0F: { /* palignr */
                    SgAsmExpression* shiftAmount;
                    switch (mmPrefix()) {
                        /* Note that getModRegRM sets the states reg and modrm. Also, standard prefixed used in the manual,
                         * "mm" refers to "mmx" registers and "xmm" refers to "sse" registers. */
                        case mmNone:
                            getModRegRM(rmMM, rmMM, QWORDT);
                            shiftAmount = getImmByte();
                            return makeInstruction(x86_palignr, "palignr", reg, modrm, shiftAmount);
                        case mmF3:
                            throw ExceptionX86("bad mm prefix F3 for opcode 0x0f3a0f", this);
                        case mm66:
                            getModRegRM(rmXMM, rmXMM, DQWORDT);
                            shiftAmount = getImmByte();
                            return makeInstruction(x86_palignr, "palignr", reg, modrm, shiftAmount);
                        case mmF2:
                            throw ExceptionX86("bad mm prefix F2 for opcode 0x0f3a0f", this);
                    }
                }
                default: {
                    char opcodestr[16];
                    sprintf(opcodestr, "0x0f3a%02x", thirdOpcodeByte);
                    throw ExceptionX86(std::string("bad or unimplemented opcode ")+opcodestr, this);
                }
            }
        }
        case 0x3B:
            throw ExceptionX86("bad opcode 0x0f3b", this);
        case 0x3C:
            throw ExceptionX86("bad opcode 0x0f3c", this);
        case 0x3D:
            throw ExceptionX86("bad opcode 0x0f3d", this);
        case 0x3E:
            throw ExceptionX86("bad opcode 0x0f3e", this);
        case 0x3F:
            throw ExceptionX86("bad opcode 0x0f3f", this);
        case 0x40:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovo, "cmovo", reg, modrm);
        case 0x41:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovno, "cmovno", reg, modrm);
        case 0x42:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovb, "cmovb", reg, modrm);
        case 0x43:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovae, "cmovae", reg, modrm);
        case 0x44:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmove, "cmove", reg, modrm);
        case 0x45:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovne, "cmovne", reg, modrm);
        case 0x46:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovbe, "cmovbe", reg, modrm);
        case 0x47:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmova, "cmova", reg, modrm);
        case 0x48:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovs, "cmovs", reg, modrm);
        case 0x49:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovns, "cmovns", reg, modrm);
        case 0x4A:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovpe, "cmovpe", reg, modrm);
        case 0x4B:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovpo, "cmovpo", reg, modrm);
        case 0x4C:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovl, "cmovl", reg, modrm);
        case 0x4D:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovge, "cmovge", reg, modrm);
        case 0x4E:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovle, "cmovle", reg, modrm);
        case 0x4F:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmovg, "cmovg", reg, modrm);
        case 0x50: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmDWord, rmXMM, V4FLOATT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movmskps, "movmskps", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f50", this);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f50", this);
                case mm66:
                    getModRegRM(rmDWord, rmXMM, V2DOUBLET);
                    if (modeField == 3) {
                        return makeInstruction(x86_movmskpd, "movmskpd", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f50", this);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f50", this);
            }
        }
        case 0x51: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_sqrtps, "sqrtps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_sqrtss, "sqrtss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_sqrtpd, "sqrtpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_sqrtsd, "sqrtsd", reg, modrm);
            }
        }
        case 0x52: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_rsqrtps, "rsqrtps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_rsqrtss, "rsqrtss", reg, modrm);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f52", this);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f52", this);
            }
        }
        case 0x53: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_rcpps, "rcpps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_rcpss, "rcpss", reg, modrm);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f53", this);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f53", this);
            }
        }
        case 0x54: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_andps, "andps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f54", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_andpd, "andpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f54", this);
            }
        }
        case 0x55: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_andnps, "andnps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f55", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_andnpd, "andnpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f55", this);
            }
        }
        case 0x56: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_orps, "orps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f56", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_orpd, "orpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f56", this);
            }
        }
        case 0x57: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_xorps, "xorps", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f57", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_xorpd, "xorpd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f57", this);
            }
        }
        case 0x58: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_addps, "addps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_addss, "addss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_addpd, "addpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_addsd, "addsd", reg, modrm);
            }
        }
        case 0x59: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_mulps, "mulps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_mulss, "mulss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_mulpd, "mulpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_mulsd, "mulsd", reg, modrm);
            }
        }
        case 0x5A: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT, V2DOUBLET);
                    return makeInstruction(x86_cvtps2pd, "cvtps2pd", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT, V2DOUBLET);
                    return makeInstruction(x86_cvtss2sd, "cvtss2sd", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET, V4FLOATT);
                    return makeInstruction(x86_cvtpd2ps, "cvtpd2ps", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET, V4FLOATT);
                    return makeInstruction(x86_cvtsd2ss, "cvtsd2ss", reg, modrm);
            }
        }
        case 0x5B: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT, V4FLOATT);
                    return makeInstruction(x86_cvtdq2ps, "cvtdq2ps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT, V4DWORDT);
                    return makeInstruction(x86_cvttps2dq, "cvttps2dq", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT, V4DWORDT);
                    return makeInstruction(x86_cvtps2dq, "cvtps2dq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f5b", this);
            }
        }
        case 0x5C: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_subps, "subps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_subss, "subss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_subpd, "subpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_subsd, "subsd", reg, modrm);
            }
        }
        case 0x5D: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_minps, "minps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_minss, "minss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_minpd, "minpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_minsd, "minsd", reg, modrm);
            }
        }
        case 0x5E: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_divps, "divps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_divss, "divss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_divpd, "divpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_divsd, "divsd", reg, modrm);
            }
        }
        case 0x5F: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_maxps, "maxps", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_maxss, "maxss", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_maxpd, "maxpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_maxsd, "maxsd", reg, modrm);
            }
        }
        case 0x60: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(x86_punpcklbw, "punpcklbw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f60", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(x86_punpcklbw, "punpcklbw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f60", this);
            }
        }
        case 0x61: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(x86_punpcklwd, "punpcklwd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f61", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(x86_punpcklwd, "punpcklwd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f61", this);
            }
        }
        case 0x62: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(x86_punpckldq, "punpckldq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f62", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(x86_punpckldq, "punpckldq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f62", this);
            }
        }
        case 0x63: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT, V8BYTET);
                    return makeInstruction(x86_packsswb, "packsswb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f63", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT, V16BYTET);
                    return makeInstruction(x86_packsswb, "packsswb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f63", this);
            }
        }
        case 0x64: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_pcmpgtb, "pcmpgtb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f64", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_pcmpgtb, "pcmpgtb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f64", this);
            }
        }
        case 0x65: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pcmpgtw, "pcmpgtw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f65", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pcmpgtw, "pcmpgtw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f65", this);
            }
        }
        case 0x66: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_pcmpgtd, "pcmpgtd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f66", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_pcmpgtd, "pcmpgtd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f66", this);
            }
        }
        case 0x67: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT, V8BYTET);
                    return makeInstruction(x86_packuswb, "packuswb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f67", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT, V16BYTET);
                    return makeInstruction(x86_packuswb, "packuswb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f67", this);
            }
        }
        case 0x68: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(x86_punpckhbw, "punpckhbw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f68", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(x86_punpckhbw, "punpckhbw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f68", this);
            }
        }
        case 0x69: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(x86_punpckhwd, "punpckhwd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f69", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(x86_punpckhwd, "punpckhwd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f69", this);
            }
        }
        case 0x6A: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(x86_punpckhdq, "punpckhdq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6a", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(x86_punpckhdq, "punpckhdq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6a", this);
            }
        }
        case 0x6B: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT, V4WORDT);
                    return makeInstruction(x86_packssdw, "packssdw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6b", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT, V8WORDT);
                    return makeInstruction(x86_packssdw, "packssdw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6b", this);
            }
        }
        case 0x6C: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f6c", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6c", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT, DQWORDT);
                    return makeInstruction(x86_punpcklqdq, "punpcklqdq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6c", this);
            }
        }
        case 0x6D: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f6d", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6d", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT, DQWORDT);
                    return makeInstruction(x86_punpckhqdq, "punpckhqdq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6d", this);
            }
        }
        case 0x6E: {
            switch (mmPrefix()) {
                case mmNone:
                    if (effectiveOperandSize()==x86_insnsize_64) {
                        getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), QWORDT);
                        return makeInstruction(x86_movq, "movq", reg, modrm);
                    } else {
                        getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), V2DWORDT);
                        return makeInstruction(x86_movd, "movd", reg, modrm);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6e", this);
                case mm66:
                    /* 0x66 is part of the opcode rather than an operand size override and used to distinguish between mm and
                     * xmm registers. The operands are 32 bits unless the REX.W bit is set, in which case they're 64 bits. */
                    operandSizeOverride = false;
                    if (effectiveOperandSize()==x86_insnsize_64) {
                        getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V2QWORDT);
                        return makeInstruction(x86_movq, "movq", reg, modrm);
                    } else {
                        getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V4DWORDT);
                        return makeInstruction(x86_movd, "movd", reg, modrm);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6e", this);
            }
        }
        case 0x6F: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_movq, "movq", reg, modrm);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_movdqu, "movdqu", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_movdqa, "movdqa", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6f", this);
            }
        }
        case 0x70: {
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_pshufw, "pshufw", reg, modrm, shufConstant);
                }
                case mmF3: {
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_pshufhw, "pshufhw", reg, modrm, shufConstant);
                }
                case mm66: {
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_pshufd, "pshufd", reg, modrm, shufConstant);
                }
                case mmF2: {
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_pshuflw, "pshuflw", reg, modrm, shufConstant);
                }
            }
        }
        case 0x71: {
            /* Group 12 */
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmReturnNull, rmMM, V4WORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 2: return makeInstruction(x86_psrlw, "psrlw", modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 4: return makeInstruction(x86_psraw, "psraw", modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 6: return makeInstruction(x86_psllw, "psllw", modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f71", this);
                case mm66: {
                    getModRegRM(rmReturnNull, rmXMM, V8WORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 2: return makeInstruction(x86_psrlw, "psrlw", modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 4: return makeInstruction(x86_psraw, "psraw", modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        case 6: return makeInstruction(x86_psllw, "psllw", modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", this);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f71", this);
            }
        }
        case 0x72: {
            /* Group 13 */
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmReturnNull, rmMM, V2DWORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 2: return makeInstruction(x86_psrld, "psrld", modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 4: return makeInstruction(x86_psrad, "psrad", modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 6: return makeInstruction(x86_pslld, "pslld", modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f72", this);
                case mm66: {
                    getModRegRM(rmReturnNull, rmXMM, V4DWORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 2: return makeInstruction(x86_psrld, "psrld", modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 4: return makeInstruction(x86_psrad, "psrad", modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        case 6: return makeInstruction(x86_pslld, "pslld", modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", this);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f72", this);
            }
        }
        case 0x73: {
            /* Group 14 */
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmReturnNull, rmMM, QWORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 2: return makeInstruction(x86_psrlq, "psrlq", modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 4: return makeInstruction(x86_psraq, "psraq", modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 6: return makeInstruction(x86_psllq, "psllq", modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f73", this);
                case mm66: {
                    getModRegRM(rmReturnNull, rmXMM, V2QWORDT);
                    if (modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                    SgAsmExpression* shiftAmount = getImmByte();
                    switch (regField) {
                        case 0:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 1:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 2:
                            return makeInstruction(x86_psrlq, "psrlq", modrm, shiftAmount);
                        case 3:
                            isSgAsmRegisterReferenceExpression(modrm)->set_type(DQWORDT);
                            return makeInstruction(x86_psrldq, "psrldq", modrm, shiftAmount);
                        case 4:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 5:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", this);
                        case 6:
                            return makeInstruction(x86_psllq, "psllq", modrm, shiftAmount);
                        case 7:
                            isSgAsmRegisterReferenceExpression(modrm)->set_type(DQWORDT);
                            return makeInstruction(x86_pslldq, "pslldq", modrm, shiftAmount);
                        default:
                            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f73", this);
            }
        }
        case 0x74: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_pcmpeqb, "pcmpeqb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f74", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_pcmpeqb, "pcmpeqb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f74", this);
            }
        }
        case 0x75: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pcmpeqw, "pcmpeqw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f75", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pcmpeqw, "pcmpeqw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f75", this);
            }
        }
        case 0x76: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_pcmpeqd, "pcmpeqd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f76", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_pcmpeqd, "pcmpeqd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f76", this);
            }
        }
        case 0x77: {
            switch (mmPrefix()) {
                case mmNone:
                    return makeInstruction(x86_emms, "emms");
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f77", this);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f77", this);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f77", this);
            }
        }
        case 0x78: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("vmread not supported", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f78", this);
                case mm66: {
                    /* Group 17 */
                    getModRegRM(rmReturnNull, rmXMM, DQWORDT);
                    SgAsmExpression* imm1 = getImmByte();
                    SgAsmExpression* imm2 = getImmByte();
                    switch (regField) {
                        case 0:
                            if (modeField == 3) {
                                return makeInstruction(x86_extrq, "extrq", modrm, imm1, imm2);
                            } else {
                                throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", this);
                            }
                        default:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", this);
                    }
                }
                case mmF2: {
                    getModRegRM(rmXMM, rmXMM, QWORDT, DQWORDT);
                    SgAsmExpression* imm1 = getImmByte();
                    SgAsmExpression* imm2 = getImmByte();
                    if (modeField == 3) {
                        return makeInstruction(x86_insertq, "insertq", reg, modrm, imm1, imm2);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", this);
                    }
                }
            }
        }
        case 0x79: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("vmwrite not supported", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f79", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, QWORDT, DQWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_extrq, "extrq", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f79", this);
                    }
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_insertq, "insertq", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f79", this);
                    }
            }
        }
        case 0x7A:
            throw ExceptionX86("bad opcode 0x0f7a", this);
        case 0x7B:
            throw ExceptionX86("bad opcode 0x0f7b", this);
        case 0x7C: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f7c", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f7c", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_haddpd, "haddpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_haddps, "haddps", reg, modrm);
            }
        }
        case 0x7D: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f7d", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f7d", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_hsubpd, "hsubpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_hsubps, "hsubps", reg, modrm);
            }
        }
        case 0x7E: {
            switch (mmPrefix()) {
                case mmNone:
                    if (effectiveOperandSize()==x86_insnsize_64) {
                        getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), QWORDT);
                        return makeInstruction(x86_movq, "movq", modrm, reg);
                    } else {
                        getModRegRM(rmMM, effectiveOperandMode(), effectiveOperandType(), V2DWORDT);
                        return makeInstruction(x86_movd, "movd", modrm, reg);
                    }
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(x86_movq, "movq", reg, modrm);
                case mm66:
                    /* 0x66 is part of the opcode rather than an operand size override and used to distinguish between mm and
                     * xmm registers. The operands are 32 bits unless the REX.W bit is set, in which case they're 64 bits. */
                    operandSizeOverride = false;
                    if (effectiveOperandSize()==x86_insnsize_64) {
                        getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V2QWORDT);
                        return makeInstruction(x86_movq, "movq", modrm, reg);
                    } else {
                        getModRegRM(rmXMM, effectiveOperandMode(), effectiveOperandType(), V4DWORDT);
                        return makeInstruction(x86_movd, "movd", modrm, reg);
                    }
                    
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f7e", this);
            }
        }
        case 0x7F: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_movq, "movq", modrm, reg);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_movdqu, "movdqu", modrm, reg);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_movdqa, "movdqa", modrm, reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f7f", this);
            }
        }
        case 0x80: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jo, "jo", imm);
        }
        case 0x81: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jno, "jno", imm);
        }
        case 0x82: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jb, "jb", imm);
        }
        case 0x83: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jae, "jae", imm);
        }
        case 0x84: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_je, "je", imm);
        }
        case 0x85: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jne, "jne", imm);
        }
        case 0x86: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jbe, "jbe", imm);
        }
        case 0x87: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_ja, "ja", imm);
        }
        case 0x88: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_js, "js", imm);
        }
        case 0x89: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jns, "jns", imm);
        }
        case 0x8A: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jpe, "jpe", imm);
        }
        case 0x8B: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jpo, "jpo", imm);
        }
        case 0x8C: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jl, "jl", imm);
        }
        case 0x8D: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jge, "jge", imm);
        }
        case 0x8E: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jle, "jle", imm);
        }
        case 0x8F: {
            SgAsmExpression* imm = getImmJz();
            branchPredictionEnabled = true;
            return makeInstruction(x86_jg, "jg", imm);
        }
        case 0x90:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_seto, "seto", modrm);
        case 0x91:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setno, "setno", modrm);
        case 0x92:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setb, "setb", modrm);
        case 0x93:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setae, "setae", modrm);
        case 0x94:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_sete, "sete", modrm);
        case 0x95:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setne, "setne", modrm);
        case 0x96:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setbe, "setbe", modrm);
        case 0x97:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_seta, "seta", modrm);
        case 0x98:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_sets, "sets", modrm);
        case 0x99:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setns, "setns", modrm);
        case 0x9A:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setpe, "setpe", modrm);
        case 0x9B:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setpo, "setpo", modrm);
        case 0x9C:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setl, "setl", modrm);
        case 0x9D:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setge, "setge", modrm);
        case 0x9E:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setle, "setle", modrm);
        case 0x9F:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_setg, "setg", modrm);
        case 0xA0:
            return makeInstruction(x86_push, "push", makeRegister(4, rmSegment));
        case 0xA1:
            return makeInstruction(x86_pop, "pop", makeRegister(4, rmSegment));
        case 0xA2:
            return makeInstruction(x86_cpuid, "cpuid");
        case 0xA3:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_bt, "bt", modrm, reg);
        case 0xA4:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_shld, "shld", modrm, reg, getImmByte());
        case 0xA5:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_shld, "shld", modrm, reg, makeRegister(1, rmLegacyByte));
        case 0xA6:
            throw ExceptionX86("bad opcode 0x0fa6", this);
        case 0xA7:
            throw ExceptionX86("bad opcode 0x0fa7", this);
        case 0xA8:
            return makeInstruction(x86_push, "push", makeRegister(5, rmSegment));
        case 0xA9:
            return makeInstruction(x86_pop, "pop", makeRegister(5, rmSegment));
        case 0xAA:
            return makeInstruction(x86_rsm, "rsm");
        case 0xAB:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_btr, "btr", modrm, reg);
        case 0xAC:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_shrd, "shrd", modrm, reg, getImmByte());
        case 0xAD:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_shrd, "shrd", modrm, reg, makeRegister(1, rmLegacyByte));
        case 0xAE:
            return decodeGroup15();
        case 0xAF:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_imul, "imul", reg, modrm);
        case 0xB0:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_cmpxchg, "cmpxchg", modrm, reg);
        case 0xB1:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_cmpxchg, "cmpxchg", modrm, reg);
        case 0xB2:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            return makeInstruction(x86_lss, "lss", reg, modrm);
        case 0xB3:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_bts, "bts", modrm, reg);
        case 0xB4:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            return makeInstruction(x86_lfs, "lfs", reg, modrm);
        case 0xB5:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            requireMemory();
            return makeInstruction(x86_lgs, "lgs", reg, modrm);
        case 0xB6:
            getModRegRM(effectiveOperandMode(), rmLegacyByte, BYTET);
            return makeInstruction(x86_movzx, "movzx", reg, modrm);
        case 0xB7:
            getModRegRM(effectiveOperandMode(), rmWord, WORDT);
            return makeInstruction(x86_movzx, "movzx", reg, modrm);
        case 0xB8: {
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            /* Here is an example of the existence of a prefix leading to two very different instructions. */
            switch (mmPrefix()) {
                case mmNone:
                    isUnconditionalJump = true;
                    return makeInstruction(x86_jmpe, "jmpe", modrm);
                case mmF3:
                    return makeInstruction(x86_popcnt, "popcnt", reg, modrm);
                default:
                    throw ExceptionX86("bad mm prefix for opcode 0x0fb8", this);
            }
        }
        case 0xB9:
            throw ExceptionX86("bad opcode 0x0fb9", this);
        case 0xBA:
            getModRegRM(rmReturnNull, effectiveOperandMode(), effectiveOperandType());
            return decodeGroup8(getImmByte());
        case 0xBB:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_btc, "btc", modrm, reg);
        case 0xBC:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_bsf, "bsf", reg, modrm);
        case 0xBD:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            if (repeatPrefix == x86_repeat_repe) return makeInstruction(x86_lzcnt, "lzcnt", reg, modrm);
            else return makeInstruction(x86_bsr, "bsr", reg, modrm);
        case 0xBE:
            getModRegRM(effectiveOperandMode(), rmLegacyByte, BYTET);
            return makeInstruction(x86_movsx, "movsx", reg, modrm);
        case 0xBF:
            getModRegRM(effectiveOperandMode(), rmWord, WORDT);
            return makeInstruction(x86_movsx, "movsx", reg, modrm);
        case 0xC0:
            getModRegRM(rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(x86_xadd, "xadd", modrm, reg);
        case 0xC1:
            getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
            return makeInstruction(x86_xadd, "xadd", modrm, reg);
        case 0xC2: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_cmpps, "cmpps", reg, modrm, getImmByte());
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, FLOATT);
                    return makeInstruction(x86_cmpss, "cmpss", reg, modrm, getImmByte());
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_cmppd, "cmppd", reg, modrm, getImmByte());
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(x86_cmpsd, "cmpsd", reg, modrm, getImmByte());
            }
        }
        case 0xC3: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(effectiveOperandMode(), effectiveOperandMode(), effectiveOperandType());
                    requireMemory();
                    return makeInstruction(x86_movnti, "movnti", modrm, reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc3", this);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0fc3", this);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc3", this);
            }
        }
        case 0xC4: {
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmMM, rmWord, WORDT, QWORDT);
                    SgAsmExpression* imm = getImmByte();
                    return makeInstruction(x86_pinsrw, "pinsrw", reg, modrm, imm);
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc4", this);
                case mm66: {
                    getModRegRM(rmXMM, rmWord, WORDT, DQWORDT);
                    SgAsmExpression* imm = getImmByte();
                    return makeInstruction(x86_pinsrw, "pinsrw", reg, modrm, imm);
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc4", this);
            }
        }
        case 0xC5: {
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmDWord, rmMM, V4WORDT, DWORDT);
                    SgAsmExpression* imm = getImmByte();
                    if (modeField == 3) {
                        return makeInstruction(x86_pextrw, "pextrw", reg, modrm, imm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc5", this);
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc5", this);
                case mm66: {
                    getModRegRM(rmDWord, rmXMM, V8WORDT, DWORDT);
                    SgAsmExpression* imm = getImmByte();
                    if (modeField == 3) {
                        return makeInstruction(x86_pextrw, "pextrw", reg, modrm, imm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc5", this);
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc5", this);
            }
        }
        case 0xC6: {
            switch (mmPrefix()) {
                case mmNone: {
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_shufps, "shufps", reg, modrm, shufConstant);
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc6", this);
                case mm66: {
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    SgAsmExpression* shufConstant = getImmByte();
                    return makeInstruction(x86_shufpd, "shufpd", reg, modrm, shufConstant);
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc6", this);
            }
        }
        case 0xC7: {
            /* Group 9 */
            getModRegRM(rmReturnNull, rmReturnNull, NULL);
            requireMemory();
            switch (regField) {
                case 1: {
                    if (effectiveOperandSize() == x86_insnsize_64) {
                        fillInModRM(rmReturnNull, DQWORDT);
                        return makeInstruction(x86_cmpxchg16b, "cmpxchg16b", modrm);
                    } else {
                        fillInModRM(rmReturnNull, QWORDT);
                        return makeInstruction(x86_cmpxchg8b, "cmpxchg8b", modrm);
                    }
                }
                case 6: {
                    fillInModRM(rmReturnNull, QWORDT);
                    switch (mmPrefix()) {
                        case mmNone: return makeInstruction(x86_vmptrld, "vmptrld", modrm);
                        case mmF3:   return makeInstruction(x86_vmxon, "vmxon", modrm);
                        case mm66:   return makeInstruction(x86_vmclear, "vmclear", modrm);
                        default:     throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc7", this);
                    }
                }
                case 7: {
                    fillInModRM(rmReturnNull, QWORDT);
                    return makeInstruction(x86_vmptrst, "vmptrst", modrm);
                }
                default:
                    throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc7", this);
            }
        }
        case 0xC8:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 0));
        case 0xC9:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 1));
        case 0xCA:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 2));
        case 0xCB:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 3));
        case 0xCC:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 4));
        case 0xCD:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 5));
        case 0xCE:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 6));
        case 0xCF:
            return makeInstruction(x86_bswap, "bswap", makeRegisterEffective(rexB, 7));
        case 0xD0: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fd0", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd0", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(x86_addsubpd, "addsubpd", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(x86_addsubps, "addsubps", reg, modrm);
            }
        }
        case 0xD1: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psrlw, "psrlw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd1", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psrlw, "psrlw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd1", this);
            }
        }
        case 0xD2: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_psrld, "psrld", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd2", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_psrld, "psrld", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd2", this);
            }
        }
        case 0xD3: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_psrlq, "psrlq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd3", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(x86_psrlq, "psrlq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd3", this);
            }
        }
        case 0xD4: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_paddq, "paddq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd4", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(x86_paddq, "paddq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd4", this);
            }
        }
        case 0xD5: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pmullw, "pmullw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd5", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pmullw, "pmullw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd5", this);
            }
        }
        case 0xD6: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fd6", this);
                case mmF3:
                    getModRegRM(rmMM, rmXMM, QWORDT, DQWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movq2dq, "movq2dq", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd6", this);
                    }
                case mm66:
                    getModRegRM(rmXMM, rmXMM, QWORDT);
                    return makeInstruction(x86_movq, "movq", modrm, reg);
                case mmF2:
                    getModRegRM(rmXMM, rmMM, DQWORDT, QWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movdq2q, "movdq2q", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd6", this);
                    }
            }
        }
        case 0xD7: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmDWord, rmMM, V8BYTET, DWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_pmovmskb, "pmovmskb", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd7", this);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd7", this);
                case mm66:
                    getModRegRM(rmDWord, rmXMM, V16BYTET, DWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_pmovmskb, "pmovmskb", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd7", this);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd7", this);
            }
        }
        case 0xD8: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_psubusb, "psubusb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd8", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_psubusb, "psubusb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd8", this);
            }
        }
        case 0xD9: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psubusw, "psubusw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd9", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psubusw, "psubusw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd9", this);
            }
        }
        case 0xDA: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_pminub, "pminub", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fda", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_pminub, "pminub", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fda", this);
            }
        }
        case 0xDB: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_pand, "pand", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdb", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_pand, "pand", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdb", this);
            }
        }
        case 0xDC: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_paddusb, "paddusb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdc", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_paddusb, "paddusb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdc", this);
            }
        }
        case 0xDD: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_paddusw, "paddusw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdd", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_paddusw, "paddusw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdd", this);
            }
        }
        case 0xDE: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_pmaxub, "pmaxub", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fde", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_pmaxub, "pmaxub", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fde", this);
            }
        }
        case 0xDF: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_pandn, "pandn", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdf", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_pandn, "pandn", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdf", this);
            }
        }
        case 0xE0: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_pavgb, "pavgb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe0", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_pavgb, "pavgb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe0", this);
            }
        }
        case 0xE1: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psraw, "psraw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe1", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psraw, "psraw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe1", this);
            }
        }
        case 0xE2: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_psrad, "psrad", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe2", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_psrad, "psrad", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe2", this);
            }
        }
        case 0xE3: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pavgw, "pavgw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe3", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pavgw, "pavgw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe3", this);
            }
        }
        case 0xE4: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pmulhuw, "pmulhuw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe4", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pmulhuw, "pmulhuw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe4", this);
            }
        }
        case 0xE5: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pmulhw, "pmulhw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe5", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pmulhw, "pmulhw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe5", this);
            }
        }
        case 0xE6: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fe6", this);
                case mmF3:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT, V2DOUBLET);
                    return makeInstruction(x86_cvtdq2pd, "cvtdq2pd", reg, modrm);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET, V2QWORDT);
                    return makeInstruction(x86_cvttpd2dq, "cvttpd2dq", reg, modrm);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, V2DOUBLET, V2QWORDT);
                    return makeInstruction(x86_cvtpd2dq, "cvtpd2dq", reg, modrm);
            }
        }
        case 0xE7: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    requireMemory();
                    return makeInstruction(x86_movntq, "movntq", modrm, reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe7", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    requireMemory();
                    return makeInstruction(x86_movntdq, "movntdq", modrm, reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe7", this);
            }
        }
        case 0xE8: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_psubsb, "psubsb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe8", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_psubsb, "psubsb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe8", this);
            }
        }
        case 0xE9: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psubsw, "psubsw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe9", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psubsw, "psubsw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe9", this);
            }
        }
        case 0xEA: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pminsw, "pminsw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fea", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pminsw, "pminsw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fea", this);
            }
        }
        case 0xEB: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_por, "por", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0feb", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_por, "por", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0feb", this);
            }
        }
        case 0xEC: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_paddsb, "paddsb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fec", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_paddsb, "paddsb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fec", this);
            }
        }
        case 0xED: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_paddsw, "paddsw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fed", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_paddsw, "paddsw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fed", this);
            }
        }
        case 0xEE: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_pmaxsw, "pmaxsw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fee", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_pmaxsw, "pmaxsw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fee", this);
            }
        }
        case 0xEF: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_pxor, "pxor", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fef", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(x86_pxor, "pxor", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fef", this);
            }
        }
        case 0xF0: {
            switch (mmPrefix()) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0ff0", this);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff0", this);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0ff0", this);
                case mmF2:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    requireMemory();
                    return makeInstruction(x86_lddqu, "lddqu", reg, modrm);
            }
        }
        case 0xF1: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psllw, "psllw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff1", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psllw, "psllw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff1", this);
            }
        }
        case 0xF2: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_pslld, "pslld", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff2", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_pslld, "pslld", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff2", this);
            }
        }
        case 0xF3: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_psllq, "psllq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff3", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(x86_psllq, "psllq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff3", this);
            }
        }
        case 0xF4: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(x86_pmuludq, "pmuludq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff4", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(x86_pmuludq, "pmuludq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff4", this);
            }
        }
        case 0xF5: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(x86_pmaddwd, "pmaddwd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff5", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(x86_pmaddwd, "pmaddwd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff5", this);
            }
        }
        case 0xF6: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(x86_psadbw, "psadbw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff6", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(x86_psadbw, "psadbw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff6", this);
            }
        }
        case 0xF7: {
            /* FIXME: The MOVNTQ and MOVNTDQ are at 0F E7 instead. This should be MASKMOVDQU. See Intel documentation.
             *        [RPM 2009-07-02] */
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movntq, "movntq", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0ff7", this);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff7", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, DQWORDT);
                    if (modeField == 3) {
                        return makeInstruction(x86_movntdq, "movntdq", reg, modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0ff7", this);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff7", this);
            }
        }
        case 0xF8: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_psubb, "psubb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff8", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_psubb, "psubb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff8", this);
            }
        }
        case 0xF9: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_psubw, "psubw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff9", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_psubw, "psubw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff9", this);
            }
        }
        case 0xFA: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_psubd, "psubd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffa", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_psubd, "psubd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffa", this);
            }
        }
        case 0xFB: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, QWORDT);
                    return makeInstruction(x86_psubq, "psubq", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffb", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(x86_psubq, "psubq", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffb", this);
            }
        }
        case 0xFC: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V8BYTET);
                    return makeInstruction(x86_paddb, "paddb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffc", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(x86_paddb, "paddb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffc", this);
            }
        }
        case 0xFD: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V4WORDT);
                    return makeInstruction(x86_paddw, "paddw", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffd", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(x86_paddw, "paddw", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffd", this);
            }
        }
        case 0xFE: {
            switch (mmPrefix()) {
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_paddd, "paddd", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffe", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_paddd, "paddd", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffe", this);
            }
        }
        case 0xFF:
            throw ExceptionX86("bad opcode 0x0fff", this);
    }

    // avoid MSCV warning by adding return stmt
    return NULL;
}

/* SSSE3 (opcode 0F38) */
SgAsmX86Instruction *
DisassemblerX86::decodeOpcode0F38()
{
    // Get the third byte of the opcode (the first two were read by the caller (decodeOpcode0F())
    uint8_t opcode = getByte();
    switch (opcode) {
        case 0x00: {
            switch (mmPrefix()) {
                /* Note that getModRegRM sets the states reg and modrm. Also, standard prefixed used in the manual, "mm"
                 * refers to "mmx" registers and "xmm" refers to "sse" registers. */
                case mmNone:
                    getModRegRM(rmMM, rmMM, V2DWORDT);
                    return makeInstruction(x86_pshufb, "pshufb", reg, modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f3800", this);
                case mm66:
                    getModRegRM(rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(x86_pshufb, "pshufb", reg, modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f3800", this);
            }
        }
        default:
            throw ExceptionX86("bad SSE3 opcode", this);
    }
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionD8()
{
    getModRegRM(rmReturnNull, rmST, FLOATT);
    if (isSgAsmMemoryReferenceExpression(modrm)) {
        isSgAsmMemoryReferenceExpression(modrm)->set_type(FLOATT);
    }
    if (modregrmByte < 0xC0) { // Using memory
        switch (regField) {
            case 0: return makeInstruction(x86_fadd, "fadd",  modrm);
            case 1: return makeInstruction(x86_fmul, "fmul",  modrm);
            case 2: return makeInstruction(x86_fcom, "fcom",  modrm);
            case 3: return makeInstruction(x86_fcomp, "fcomp", modrm);
            case 4: return makeInstruction(x86_fsub, "fsub",  modrm);
            case 5: return makeInstruction(x86_fsubr, "fsubr", modrm);
            case 6: return makeInstruction(x86_fdiv, "fdiv",  modrm);
            case 7: return makeInstruction(x86_fdivr, "fdivr", modrm);
            default: {
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                /* avoid MSCV warning by adding return stmt */
                return NULL;
            }
        }
    } else { // Two-operand register forms
        switch (regField) {
            case 0: return makeInstruction(x86_fadd, "fadd",   makeRegister(0, rmST), modrm);
            case 1: return makeInstruction(x86_fmul, "fmul",   makeRegister(0, rmST), modrm);
            case 2: return makeInstruction(x86_fcom, "fcom",   makeRegister(0, rmST), modrm);
            case 3: return makeInstruction(x86_fcomp, "fcomp", makeRegister(0, rmST), modrm);
            case 4: return makeInstruction(x86_fsub, "fsub",   makeRegister(0, rmST), modrm);
            case 5: return makeInstruction(x86_fsubr, "fsubr", makeRegister(0, rmST), modrm);
            case 6: return makeInstruction(x86_fdiv, "fdiv",   makeRegister(0, rmST), modrm);
            case 7: return makeInstruction(x86_fdivr, "fdivr", makeRegister(0, rmST), modrm);
            default: {
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
                /* avoid MSCV warning by adding return stmt */
                return NULL;
            }
        }
    }
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionD9()
{
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) {
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
        ASSERT_not_null(mr);
        switch (regField) {
            case 0:
                mr->set_type(FLOATT);
                return makeInstruction(x86_fld, "fld", modrm);
            case 1:
                throw ExceptionX86("bad ModR/M for x87 opcode 0xd9", this);
            case 2:
                mr->set_type(FLOATT);
                return makeInstruction(x86_fst, "fst", modrm);
            case 3:
                mr->set_type(FLOATT);
                return makeInstruction(x86_fstp, "fstp", modrm);
            case 4:
                mr->set_type(BYTET);
                return makeInstruction(x86_fldenv, "fldenv", modrm);
            case 5:
                mr->set_type(WORDT);
                return makeInstruction(x86_fldcw, "fldcw", modrm);
            case 6:
                mr->set_type(BYTET);
                return makeInstruction(x86_fnstenv, "fnstenv", modrm);
            case 7:
                mr->set_type(WORDT);
                return makeInstruction(x86_fnstcw, "fnstcw", modrm);
            default:
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
      }
    } else if (regField == 0 || regField == 1) { // FLD and FXCH on registers
        modrm = makeModrmNormal(rmST, NULL);
        switch (regField) {
            case 0: return makeInstruction(x86_fld, "fld", modrm);
            case 1: return makeInstruction(x86_fxch, "fxch", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else {
        switch (modregrmByte) {
            case 0xD0: return makeInstruction(x86_fnop, "fnop");
            case 0xE0: return makeInstruction(x86_fchs, "fchs");
            case 0xE1: return makeInstruction(x86_fabs, "fabs");
            case 0xE4: return makeInstruction(x86_ftst, "ftst");
            case 0xE5: return makeInstruction(x86_fxam, "fxam");
            case 0xE8: return makeInstruction(x86_fld1, "fld1");
            case 0xE9: return makeInstruction(x86_fldl2t, "fldl2t");
            case 0xEA: return makeInstruction(x86_fldl2e, "fldl2e");
            case 0xEB: return makeInstruction(x86_fldpi, "fldpi");
            case 0xEC: return makeInstruction(x86_fldlg2, "fldlg2");
            case 0xED: return makeInstruction(x86_fldln2, "fldln2");
            case 0xEE: return makeInstruction(x86_fldz, "fldz");
            case 0xF0: return makeInstruction(x86_f2xm1, "f2xm1");
            case 0xF1: return makeInstruction(x86_fyl2x, "fyl2x");
            case 0xF2: return makeInstruction(x86_fptan, "fptan");
            case 0xF3: return makeInstruction(x86_fpatan, "fpatan");
            case 0xF4: return makeInstruction(x86_fxtract, "fxtract");
            case 0xF5: return makeInstruction(x86_fprem1, "fprem1");
            case 0xF6: return makeInstruction(x86_fdecstp, "fdecstp");
            case 0xF7: return makeInstruction(x86_fincstp, "fincstp");
            case 0xF8: return makeInstruction(x86_fprem, "fprem");
            case 0xF9: return makeInstruction(x86_fyl2xp1, "fyl2xp1");
            case 0xFA: return makeInstruction(x86_fsqrt, "fsqrt");
            case 0xFB: return makeInstruction(x86_fsincos, "fsincos");
            case 0xFC: return makeInstruction(x86_frndint, "frndint");
            case 0xFD: return makeInstruction(x86_fscale, "fscale");
            case 0xFE: return makeInstruction(x86_fsin, "fsin");
            case 0xFF: return makeInstruction(x86_fcos, "fcos");
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xd9", this);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDA()
{
    getModRegRM(rmReturnNull, rmReturnNull, DWORDT);
    if (modeField < 3) {
        switch (regField) {
            case 0: return makeInstruction(x86_fiadd, "fiadd", modrm);
            case 1: return makeInstruction(x86_fimul, "fimul", modrm);
            case 2: return makeInstruction(x86_ficom, "ficom", modrm);
            case 3: return makeInstruction(x86_ficomp, "ficomp", modrm);
            case 4: return makeInstruction(x86_fisub, "fisub", modrm);
            case 5: return makeInstruction(x86_fisubr, "fisubr", modrm);
            case 6: return makeInstruction(x86_fidiv, "fidiv", modrm);
            case 7: return makeInstruction(x86_fidivr, "fidivr", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else if (regField < 4) { // FCMOV{B,E,BE,U}
        modrm = makeModrmRegister(rmST);
        switch (regField) {
            case 0: return makeInstruction(x86_fcmovb, "fcmovb", makeRegister(0, rmST), modrm);
            case 1: return makeInstruction(x86_fcmove, "fcmove", makeRegister(0, rmST), modrm);
            case 2: return makeInstruction(x86_fcmovbe, "fcmovbe", makeRegister(0, rmST), modrm);
            case 3: return makeInstruction(x86_fcmovu, "fcmovu", makeRegister(0, rmST), modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else {
        switch (modregrmByte) {
            case 0xE9: return makeInstruction(x86_fucompp, "fucompp");
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xda", this);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDB()
{
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) {
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
        ASSERT_not_null(mr);
        if (regField <= 3) {
            mr->set_type(DWORDT);
        } else {
            mr->set_type(LDOUBLET);
        }
        switch (regField) {
            case 0: return makeInstruction(x86_fild, "fild", modrm);
            case 1: return makeInstruction(x86_fisttp, "fisttp", modrm);
            case 2: return makeInstruction(x86_fist, "fist", modrm);
            case 3: return makeInstruction(x86_fistp, "fistp", modrm);
            case 4: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", this);
            case 5: return makeInstruction(x86_fld, "fld", modrm);
            case 6: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", this);
            case 7: return makeInstruction(x86_fstp, "fstp", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else if (regField <= 3 || regField == 5 || regField == 6) { // FCMOV{NB,NE,NBE,NU}, FUCOMI, FCOMI
        modrm = makeModrmNormal(rmST, NULL);
        switch (regField) {
            case 0: return makeInstruction(x86_fcmovnb, "fcmovnb", makeRegister(0, rmST), modrm);
            case 1: return makeInstruction(x86_fcmovne, "fcmovne", makeRegister(0, rmST), modrm);
            case 2: return makeInstruction(x86_fcmovnbe, "fcmovnbe", makeRegister(0, rmST), modrm);
            case 3: return makeInstruction(x86_fcmovnu, "fcmovnu", makeRegister(0, rmST), modrm);
            case 5: return makeInstruction(x86_fucomi, "fucomi", makeRegister(0, rmST), modrm);
            case 6: return makeInstruction(x86_fcomi, "fcomi", makeRegister(0, rmST), modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
      }
    } else {
        switch (modregrmByte) {
            case 0xE2: return makeInstruction(x86_fnclex, "fnclex");
            case 0xE3: return makeInstruction(x86_fninit, "fninit");
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", this);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDC()
{
    getModRegRM(rmReturnNull, rmST, DOUBLET);
    if (modeField < 3) { // Using memory
        switch (regField & 7) {
            case 0: return makeInstruction(x86_fadd, "fadd", modrm);
            case 1: return makeInstruction(x86_fmul, "fmul", modrm);
            case 2: return makeInstruction(x86_fcom, "fcom", modrm);
            case 3: return makeInstruction(x86_fcomp, "fcomp", modrm);
            case 4: return makeInstruction(x86_fsub, "fsub", modrm);
            case 5: return makeInstruction(x86_fsubr, "fsubr", modrm);
            case 6: return makeInstruction(x86_fdiv, "fdiv", modrm);
            case 7: return makeInstruction(x86_fdivr, "fdivr", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else { // Two-operand register forms
        switch (regField & 7) {
            case 0: return makeInstruction(x86_fadd,  "fadd",  modrm, makeRegister(0, rmST));
            case 1: return makeInstruction(x86_fmul,  "fmul",  modrm, makeRegister(0, rmST));
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdc", this);
            case 3: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdc", this);
            case 4: return makeInstruction(x86_fsubr, "fsubr", modrm, makeRegister(0, rmST));
            case 5: return makeInstruction(x86_fsub,  "fsub",  modrm, makeRegister(0, rmST));
            case 6: return makeInstruction(x86_fdivr, "fdivr", modrm, makeRegister(0, rmST));
            case 7: return makeInstruction(x86_fdiv,  "fdiv",  modrm, makeRegister(0, rmST));
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDD()
{
    getModRegRM(rmReturnNull, rmST, NULL);
    if (modeField < 3) { // Using memory
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
        ASSERT_not_null(mr);
        switch (regField) {
            case 0:
                mr->set_type(DOUBLET);
                return makeInstruction(x86_fld, "fld", modrm);
            case 1:
                mr->set_type(QWORDT);
                return makeInstruction(x86_fisttp, "fisttp", modrm);
            case 2:
                mr->set_type(DOUBLET);
                return makeInstruction(x86_fst, "fst", modrm);
            case 3:
                mr->set_type(DOUBLET);
                return makeInstruction(x86_fstp, "fstp", modrm);
            case 4:
                mr->set_type(BYTET);
                return makeInstruction(x86_frstor, "frstor", modrm);
            case 5:
                throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", this);
            case 6:
                mr->set_type(BYTET);
                return makeInstruction(x86_fnsave, "fnsave", modrm);
            case 7:
                mr->set_type(WORDT);
                return makeInstruction(x86_fnstsw, "fnstsw", modrm);
            default:
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else { // Register forms
        switch (regField) {
            case 0: return makeInstruction(x86_ffree, "ffree", modrm);
            case 1: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", this);
            case 2: return makeInstruction(x86_fst, "fst", modrm);
            case 3: return makeInstruction(x86_fstp, "fstp", modrm);
            case 4: return makeInstruction(x86_fucom, "fucom", modrm, makeRegister(0, rmST));
            case 5: return makeInstruction(x86_fucomp, "fucomp", modrm, makeRegister(0, rmST));
            case 6: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", this);
            case 7: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", this);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDE()
{
    getModRegRM(rmReturnNull, rmST, WORDT);
    if (modeField < 3) { // Using memory
        switch (regField & 7) {
            case 0: return makeInstruction(x86_fiadd, "fiadd", modrm);
            case 1: return makeInstruction(x86_fimul, "fimul", modrm);
            case 2: return makeInstruction(x86_ficom, "ficom", modrm);
            case 3: return makeInstruction(x86_ficomp, "ficomp", modrm);
            case 4: return makeInstruction(x86_fisub, "fisub", modrm);
            case 5: return makeInstruction(x86_fisubr, "fisubr", modrm);
            case 6: return makeInstruction(x86_fidiv, "fidiv", modrm);
            case 7: return makeInstruction(x86_fidivr, "fidivr", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else {
        switch (regField & 7) {
            case 0: return makeInstruction(x86_faddp, "faddp", modrm, makeRegister(0, rmST));
            case 1: return makeInstruction(x86_fmulp, "fmulp", modrm, makeRegister(0, rmST));
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xde", this);
            case 3: {
                switch (modregrmByte) {
                    case 0xD9: delete modrm; delete reg; return makeInstruction(x86_fcompp, "fcompp");
                    default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xde", this);
                }
            }
            case 4: return makeInstruction(x86_fsubrp, "fsubrp", modrm, makeRegister(0, rmST));
            case 5: return makeInstruction(x86_fsubp, "fsubp", modrm, makeRegister(0, rmST));
            case 6: return makeInstruction(x86_fdivrp, "fdivrp", modrm, makeRegister(0, rmST));
            case 7: return makeInstruction(x86_fdivp, "fdivp", modrm, makeRegister(0, rmST));
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeX87InstructionDF()
{
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    if (modeField < 3) { // Using memory
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(modrm);
        ASSERT_not_null(mr);
        switch (regField) {
            case 0: mr->set_type(WORDT); return makeInstruction(x86_fild, "fild", modrm);
            case 1: mr->set_type(WORDT); return makeInstruction(x86_fisttp, "fisttp", modrm);
            case 2: mr->set_type(WORDT); return makeInstruction(x86_fist, "fist", modrm);
            case 3: mr->set_type(WORDT); return makeInstruction(x86_fistp, "fistp", modrm);
            case 4: mr->set_type(BYTET); return makeInstruction(x86_fbld, "fbld", modrm);
            case 5: mr->set_type(QWORDT); return makeInstruction(x86_fild, "fild", modrm);
            case 6: mr->set_type(BYTET); return makeInstruction(x86_fbstp, "fbstp", modrm);
            case 7: mr->set_type(QWORDT); return makeInstruction(x86_fistp, "fistp", modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    } else {
        modrm = makeModrmNormal(rmST, NULL);
        switch (regField) {
            case 0: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
            case 1: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
            case 3: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
            case 4: {
                if (modregrmByte == 0xE0) {
                    return makeInstruction(x86_fnstsw, "fnstsw", makeRegister(0, rmWord));
                } else {
                    throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
                }
            }
            case 5: return makeInstruction(x86_fucomip, "fucomip", makeRegister(0, rmST), modrm);
            case 6: return makeInstruction(x86_fcomip, "fcomip", makeRegister(0, rmST), modrm);
            case 7: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", this);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup1(SgAsmExpression* imm)
{
    switch (regField) {
        case 0: return makeInstruction(x86_add, "add", modrm, imm);
        case 1: return makeInstruction(x86_or, "or", modrm, imm);
        case 2: return makeInstruction(x86_adc, "adc", modrm, imm);
        case 3: return makeInstruction(x86_sbb, "sbb", modrm, imm);
        case 4: return makeInstruction(x86_and, "and", modrm, imm);
        case 5: return makeInstruction(x86_sub, "sub", modrm, imm);
        case 6: return makeInstruction(x86_xor, "xor", modrm, imm);
        case 7: return makeInstruction(x86_cmp, "cmp", modrm, imm);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup1a()
{
    if (regField != 0)
        throw ExceptionX86("bad ModR/M value for Group 1a opcode", this);
    return makeInstruction(x86_pop, "pop", modrm);
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup2(SgAsmExpression* count)
{
    switch (regField) {
        case 0: return makeInstruction(x86_rol, "rol", modrm, count);
        case 1: return makeInstruction(x86_ror, "ror", modrm, count);
        case 2: return makeInstruction(x86_rcl, "rcl", modrm, count);
        case 3: return makeInstruction(x86_rcr, "rcr", modrm, count);
        case 4: return makeInstruction(x86_shl, "shl", modrm, count);
        case 5: return makeInstruction(x86_shr, "shr", modrm, count);
        case 6: return makeInstruction(x86_shl, "shl", modrm, count);
        case 7: return makeInstruction(x86_sar, "sar", modrm, count);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup3(SgAsmExpression* immMaybe)
{
    switch (regField) {
        case 0:
        case 1:
            ASSERT_require(immMaybe);
            return makeInstruction(x86_test, "test", modrm, immMaybe);
        case 2:
            return makeInstruction(x86_not, "not", modrm);
        case 3:
            return makeInstruction(x86_neg, "neg", modrm);
        case 4:
            return makeInstruction(x86_mul, "mul", modrm);
        case 5:
            return makeInstruction(x86_imul, "imul", modrm);
        case 6:
            return makeInstruction(x86_div, "div", modrm);
        case 7:
            return makeInstruction(x86_idiv, "idiv", modrm);
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup4()
{
    switch (regField) {
        case 0: return makeInstruction(x86_inc, "inc", modrm);
        case 1: return makeInstruction(x86_dec, "dec", modrm);
        default: throw ExceptionX86("bad ModR/M value for Group 4 opcode", this);
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup5()
{
    switch (regField) {
        case 0:
            return makeInstruction(x86_inc, "inc", modrm);
        case 1:
            return makeInstruction(x86_dec, "dec", modrm);
        case 2:
            return makeInstruction(x86_call, "call", modrm);
        case 3:
            return makeInstruction(x86_farcall, "farCall", modrm);
        case 4:
            isUnconditionalJump = true;
            return makeInstruction(x86_jmp, "jmp", modrm);
        case 5:
            isUnconditionalJump = true;
            return makeInstruction(x86_farjmp, "farJmp", modrm);
        case 6:
            return makeInstruction(x86_push, "push", modrm);
        case 7:
            throw ExceptionX86("bad ModR/M value for Group 5 opcode", this);
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup6()
{
    switch (regField) {
        case 0: return makeInstruction(x86_sldt, "sldt", modrm); // FIXME adjust register size
        case 1: return makeInstruction(x86_str, "str", modrm); // FIXME adjust register size
        case 2: return makeInstruction(x86_lldt, "lldt", modrm);
        case 3: return makeInstruction(x86_ltr, "ltr", modrm);
        case 4: return makeInstruction(x86_verr, "verr", modrm);
        case 5: return makeInstruction(x86_verw, "verw", modrm);
        case 6: throw ExceptionX86("bad ModR/M value for Group 6 opcode", this);
        case 7: throw ExceptionX86("bad ModR/M value for Group 6 opcode", this);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup7()
{
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    switch (regField) {
        case 0: {
            if (modeField == 3) {
                switch (rmField) {
                    case 1: return makeInstruction(x86_vmcall, "vmcall");
                    case 2: return makeInstruction(x86_vmlaunch, "vmlaunch");
                    case 3: return makeInstruction(x86_vmresume, "vmresume");
                    case 4: return makeInstruction(x86_vmxoff, "vmxoff");
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", this);
                }
            } else {
                fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(x86_sgdt, "sgdt", modrm);
            }
        }
        case 1: {
            if (modeField == 3) {
                switch (rmField) {
                    case 0: return makeInstruction(x86_monitor, "monitor");
                    case 1: return makeInstruction(x86_mwait, "mwait");
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", this);
                }
            } else {
                fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(x86_sidt, "sidt", modrm);
            }
        }
        case 2: {
            if (modeField ==3) {
                switch (rmField) {
                    case 0: return makeInstruction(x86_xgetbv, "xgetbv");
                    case 1: return makeInstruction(x86_xsetbv, "xsetbv");
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", this);
                }
            } else {
                fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(x86_lgdt, "lgdt", modrm);
            }
        }
        case 3: {
            if (modeField == 3) {
                switch (rmField) {
                    case 0: return makeInstruction(x86_vmrun, "vmrun");
                    case 1: return makeInstruction(x86_vmmcall, "vmmcall");
                    case 2: return makeInstruction(x86_vmload, "vmload");
                    case 3: return makeInstruction(x86_vmsave, "vmsave");
                    case 4: return makeInstruction(x86_stgi, "stgi");
                    case 5: return makeInstruction(x86_clgi, "clgi");
                    case 6: return makeInstruction(x86_skinit, "skinit");
                    case 7: return makeInstruction(x86_invlpga, "invlpga");
                    default: ASSERT_not_reachable("invalid rm field: " + StringUtility::numberToString(rmField));
                }
            } else {
                fillInModRM(rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(x86_lidt, "lidt", modrm);
            }
        }
        case 4: {
            fillInModRM(effectiveOperandMode(), WORDT);
            return makeInstruction(x86_smsw, "smsw", modrm);
        }
        case 5:
            throw ExceptionX86("bad ModR/M value for Group 7 opcode", this);
        case 6: {
            fillInModRM(rmWord, WORDT);
            return makeInstruction(x86_lmsw, "lmsw", modrm);
        }
        case 7: {
            if (modeField == 3) {
                switch (rmField) {
                    case 0: return makeInstruction(x86_swapgs, "swapgs");
                    case 1: return makeInstruction(x86_rdtscp, "rdtscp");
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", this);
                }
            } else {
                fillInModRM(rmReturnNull, BYTET);
                return makeInstruction(x86_invlpg, "invlpg", modrm);
            }
        }
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup8(SgAsmExpression* imm)
{
    switch (regField) {
        case 0: throw ExceptionX86("bad ModR/M value for Group 8 opcode", this);
        case 1: throw ExceptionX86("bad ModR/M value for Group 8 opcode", this);
        case 2: throw ExceptionX86("bad ModR/M value for Group 8 opcode", this);
        case 3: throw ExceptionX86("bad ModR/M value for Group 8 opcode", this);
        case 4: return makeInstruction(x86_bt, "bt", modrm, imm);
        case 5: return makeInstruction(x86_bts, "bts", modrm, imm);
        case 6: return makeInstruction(x86_btr, "btr", modrm, imm);
        case 7: return makeInstruction(x86_btc, "btc", modrm, imm);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup11(SgAsmExpression* imm)
{
    switch (regField) {
        case 0: return makeInstruction(x86_mov, "mov", modrm, imm);
        default: throw ExceptionX86("bad ModR/M value for Group 11 opcode", this);
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup15()
{
    getModRegRM(rmReturnNull, rmReturnNull, NULL);
    switch (regField) {
        case 0:
            requireMemory();
            fillInModRM(rmReturnNull, BYTET);
            return makeInstruction(x86_fxsave, "fxsave", modrm);
        case 1:
            requireMemory();
            fillInModRM(rmReturnNull, BYTET);
            return makeInstruction(x86_fxrstor, "fxrstor", modrm);
        case 2:
            requireMemory();
            fillInModRM(rmReturnNull, DWORDT);
            return makeInstruction(x86_ldmxcsr, "ldmxcsr", modrm);
        case 3:
            requireMemory();
            fillInModRM(rmReturnNull, DWORDT);
            return makeInstruction(x86_stmxcsr, "stmxcsr", modrm);
        case 4:
            requireMemory();
            fillInModRM(rmReturnNull, BYTET);
            return makeInstruction(x86_xsave, "xsave", modrm);
        case 5:
            if (modeField == 3) {
                return makeInstruction(x86_lfence, "lfence");
            } else {
                return makeInstruction(x86_xrstor, "xrstor", modrm);
            }
        case 6:
            if (modeField == 3) {
                return makeInstruction(x86_mfence, "mfence");
            } else {
                throw ExceptionX86("bad ModR/M value for Group 15 opcode", this);
            }
        case 7:
            if (modeField == 3) {
                return makeInstruction(x86_sfence, "sfence");
            } else {
                fillInModRM(rmReturnNull, BYTET);
                return makeInstruction(x86_clflush, "clflush", modrm);
            }
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroup16()
{
    requireMemory();
    switch (regField) {
        case 0: return makeInstruction(x86_prefetchnta, "prefetchnta", modrm);
        case 1: return makeInstruction(x86_prefetcht0, "prefetcht0", modrm);
        case 2: return makeInstruction(x86_prefetcht1, "prefetcht1", modrm);
        case 3: return makeInstruction(x86_prefetcht2, "prefetcht2", modrm);
        default: return makeInstruction(x86_prefetch, "prefetch", modrm);
    }
}

SgAsmX86Instruction *
DisassemblerX86::decodeGroupP()
{
    getModRegRM(rmReturnNull, rmLegacyByte, BYTET);
    requireMemory();
    switch (regField) {
        case 0: return makeInstruction(x86_prefetch, "prefetch", modrm);
        case 1: return makeInstruction(x86_prefetchw, "prefetchw", modrm);
        case 3: return makeInstruction(x86_prefetchw, "prefetchw", modrm);
        default: return makeInstruction(x86_prefetch, "prefetch", modrm);
    }
}

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::DisassemblerX86);
#endif

#endif
