#include "sage3basic.h"
#include "InstructionEnumsX86.h"
#include "BinaryLoader.h"

#include <boost/foreach.hpp>
#include <Sawyer/CommandLine.h>                         // needed for CommandLine::Parser.programName(), i.e., argv[0]

namespace rose {
namespace SageBuilderAsm {

SgBinaryComposite *
buildBinaryComposite(const std::string &fileName) {
    static const bool DONT_DISASSEMBLE = true;
    SgBinaryComposite *retval = NULL;

    // Build argument string so frontend can parse it again!  Not sure why the API was designed like this, but that's what
    // we're stuck with.  Also, the first argument must be the name of this program, so we use Sawyer's method to get it since
    // we don't have a readily available argv.
    std::vector<std::string> args;
    args.push_back(Sawyer::CommandLine::Parser().programName());
    args.push_back("-rose:read_executable_file_format_only"); // i.e., don't disassemble
    //args.push_back("--");
    args.push_back(fileName);
    
    if (SgProject *project = SageInterface::getProject()) {
        // We already have a project, so we cannot call frontend() again.
        retval = new SgBinaryComposite(args, project);
        BinaryLoader::load(retval, DONT_DISASSEMBLE);
    } else {
        // No project yet, so just call frontend()
        project = frontend(args);
        retval = SageInterface::querySubTree<SgBinaryComposite>(project).front();
        ASSERT_not_null(retval);
    }
    return retval;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
buildSgAsmx86RegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
buildSgAsmArmRegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
buildSgAsmMipsRegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
buildSgAsmPowerpcRegisterReferenceExpression(const RegisterDescriptor &desc)
{
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmX86Instruction*
buildx86Instruction(X86InstructionKind kind) {
    return buildX86Instruction(kind);
}

SgAsmX86Instruction*
buildX86Instruction(X86InstructionKind kind) {
    // These are the default values used for the construction of new instructions.
    rose_addr_t address = 0;
    std::string mnemonic = "";
    X86InstructionSize baseSize  = x86_insnsize_none;
    X86InstructionSize operandSize = x86_insnsize_none; 
    X86InstructionSize addressSize = x86_insnsize_none;

    SgAsmX86Instruction* instruction = new SgAsmX86Instruction(address, mnemonic, kind, baseSize, operandSize, addressSize);

    // This should not have been set yet.
    ASSERT_require(instruction->get_operandList() == NULL);

    // All instructions are required to have a valid SgAsmOperandList pointer.
    instruction->set_operandList(new SgAsmOperandList);
    instruction->get_operandList()->set_parent(instruction);

    return instruction;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmX86Instruction*
buildx86Instruction(X86InstructionKind kind, SgAsmExpression *operand) {
    return buildX86Instruction(kind, operand);
}

SgAsmX86Instruction*
buildX86Instruction(X86InstructionKind kind, SgAsmExpression *operand)
{
    SgAsmX86Instruction* instruction = buildX86Instruction(kind);
    appendOperand(instruction,operand);
    return instruction;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmX86Instruction*
buildx86Instruction(X86InstructionKind kind, SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildX86Instruction(kind, lhs, rhs);
}

SgAsmX86Instruction*
buildX86Instruction(X86InstructionKind kind, SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmX86Instruction* instruction = buildX86Instruction(kind);
    appendOperand(instruction,lhs);
    appendOperand(instruction,rhs);
    return instruction;
}

SgAsmIntegerValueExpression*
buildValueInteger(uint64_t value, SgAsmType *type) {
    return new SgAsmIntegerValueExpression(value, type);
}

SgAsmIntegerValueExpression*
buildValueInteger(const Sawyer::Container::BitVector &bv, SgAsmType *type) {
    return new SgAsmIntegerValueExpression(bv, type);
}

SgAsmIntegerValueExpression*
buildValueU1(bool x) {
    return buildValueInteger(x ? 1 : 0, buildTypeU1());
}

SgAsmIntegerValueExpression*
buildValueU8(uint8_t x) {
    return buildValueInteger(x, buildTypeU8());
}

SgAsmIntegerValueExpression*
buildValueU16(uint16_t x) {
    return buildValueInteger(x, buildTypeU16());
}

SgAsmIntegerValueExpression*
buildValueU32(uint32_t x) {
    return buildValueInteger(x, buildTypeU32());
}

SgAsmIntegerValueExpression*
buildValueU64(uint64_t x) {
    return buildValueInteger(x, buildTypeU64());
}

SgAsmIntegerValueExpression*
buildValueI8(int8_t x) {
    return buildValueInteger((int64_t)x, buildTypeI8());
}

SgAsmIntegerValueExpression*
buildValueI16(int16_t x) {
    return buildValueInteger((int64_t)x, buildTypeI16());
}

SgAsmIntegerValueExpression*
buildValueI32(int32_t x) {
    return buildValueInteger((int64_t)x, buildTypeI32());
}

SgAsmIntegerValueExpression*
buildValueI64(int64_t x) {
    return buildValueInteger(x, buildTypeI64());
}

SgAsmFloatValueExpression*
buildValueFloat(double x, SgAsmType *type) {
    ASSERT_not_null(type);
    return new SgAsmFloatValueExpression(x, type);
}

SgAsmFloatValueExpression*
buildValueFloat(const Sawyer::Container::BitVector &bv, SgAsmType *type) {
    ASSERT_not_null(type);
    ASSERT_require(bv.size() == type->get_nBits());
    return new SgAsmFloatValueExpression(bv, type);
}

SgAsmFloatValueExpression*
buildValueIeee754Binary32(double x) {
    return buildValueFloat(x, buildIeee754Binary32());
}

SgAsmFloatValueExpression*
buildValueIeee754Binary64(double x) {
    return buildValueFloat(x, buildIeee754Binary64());
}

SgAsmIntegerValueExpression*
buildValueX86Byte(uint8_t x) {
    return buildValueU8(x);
}

SgAsmIntegerValueExpression*
buildValueX86Word(uint16_t x) {
    return buildValueU16(x);
}

SgAsmIntegerValueExpression*
buildValueX86DWord(uint32_t x) {
    return buildValueU32(x);
}

SgAsmIntegerValueExpression*
buildValueX86QWord(uint64_t x) {
    return buildValueU64(x);
}

SgAsmFloatValueExpression*
buildValueX86Float32(double x) {
    return buildValueIeee754Binary32(x);
}

SgAsmFloatValueExpression*
buildValueX86Float64(double x) {
    return buildValueIeee754Binary64(x);
}

SgAsmFloatValueExpression*
buildValueX86Float80(double x) {
    return new SgAsmFloatValueExpression(x, buildTypeX86Float80());
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
buildAsmDWordValue(uint32_t val) {
    return buildValueX86DWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
buildAsmByteValue(uint8_t val) {
    return buildValueX86Byte(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
buildAsmWordValue(uint16_t val) {
    return buildValueX86Word(val);
}

SgAsmIntegerType*
buildTypeU1() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 1, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
buildTypeU8() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 8, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
buildTypeU16() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 16, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
buildTypeU32() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 32, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
buildTypeU64() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 64, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
buildTypeI8() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 8, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
buildTypeI16() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 16, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
buildTypeI32() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 32, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
buildTypeI64() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 64, true /*signed*/));
    return cached;
}

SgAsmFloatType*
buildIeee754Binary32() {
    static SgAsmFloatType *cached = NULL;
    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 32,
                                                    SgAsmFloatType::BitRange::baseSize(0, 23), // significand
                                                    SgAsmFloatType::BitRange::baseSize(23, 8), // exponent
                                                    31,                                        // sign bit
                                                    127,                                       // exponent bias
                                                    SgAsmFloatType::NORMALIZED_SIGNIFICAND|SgAsmFloatType::GRADUAL_UNDERFLOW);
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmFloatType*
buildIeee754Binary64() {
    static SgAsmFloatType *cached = NULL;
    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 64,
                                                    SgAsmFloatType::BitRange::baseSize(0, 52),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(52, 11), // exponent
                                                    63,                                         // sign bit
                                                    1023,                                       // exponent bias
                                                    SgAsmFloatType::NORMALIZED_SIGNIFICAND|SgAsmFloatType::GRADUAL_UNDERFLOW);
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmFloatType*
buildIeee754Binary80() {
    static SgAsmFloatType *cached = NULL;
    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 80,
                                                    SgAsmFloatType::BitRange::baseSize(0, 64),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(64, 15), // exponent
                                                    79,                                         // sign bit
                                                    16383,                                      // exponent bias
                                                    SgAsmFloatType::NORMALIZED_SIGNIFICAND|SgAsmFloatType::GRADUAL_UNDERFLOW);
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmVectorType*
buildTypeVector(size_t nElmts, SgAsmType *elmtType) {
    return SgAsmType::registerOrDelete(new SgAsmVectorType(nElmts, elmtType));
}

SgAsmIntegerType*
buildTypeX86Byte() {
    return buildTypeU8();
}

SgAsmIntegerType*
buildTypeX86Word() {
    return buildTypeU16();
}

SgAsmIntegerType*
buildTypeX86DoubleWord() {
    return buildTypeU32();
}

SgAsmIntegerType*
buildTypeX86QuadWord() {
    return buildTypeU64();
}

SgAsmFloatType*
buildTypeX86Float32() {
    return buildIeee754Binary32();
}

SgAsmFloatType*
buildTypeX86Float64() {
    return buildIeee754Binary64();
}

SgAsmFloatType*
buildTypeX86Float80() {
    return buildIeee754Binary80();
}

// M68k 96-bit "extended-precision real format"
// The 16 bits at [64-79] are always zero.
SgAsmFloatType*
buildTypeM68kFloat96() {
    static SgAsmFloatType *cached = NULL;
    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 96,
                                                    SgAsmFloatType::BitRange::baseSize(0, 64),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(80, 15), // exponent
                                                    95,                                         // sign bit
                                                    16383,                                      // exponent bias
                                                    SgAsmFloatType::NORMALIZED_SIGNIFICAND|SgAsmFloatType::GRADUAL_UNDERFLOW);
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmVectorType*
buildTypeX86DoubleQuadWord() {
    SgAsmType *quadword = buildTypeX86QuadWord();
    return SgAsmType::registerOrDelete(new SgAsmVectorType(2, quadword));
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerType*
buildAsmTypeByte() {
    return buildTypeX86Byte();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
buildAsmTypeSingleFloat() {
    return buildTypeX86Float32();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
buildAsmTypeDoubleFloat() {
    return buildTypeX86Float64();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
buildAsmType80bitFloat() {
    return buildTypeX86Float80();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmVectorType*
buildAsmTypeDoubleQuadWord() {
    return buildTypeX86DoubleQuadWord();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmInstruction*
buildMultibyteNopInstruction(int n) {
    return buildX86MultibyteNopInstruction(n);
}

SgAsmX86Instruction*
buildX86MultibyteNopInstruction(size_t nBytes) {
    ASSERT_require(nBytes > 0);
    ASSERT_require(nBytes <= 9);

    SgAsmX86Instruction *instruction = new SgAsmX86Instruction(0, "nop", x86_nop,
                                                               x86_insnsize_32, x86_insnsize_32, x86_insnsize_32);

    // Build a simple version of multi-byte nop using repeated prefixes.
    SgUnsignedCharList insnbuf;
    for (size_t i = 1; i < nBytes; i++)
        insnbuf.push_back(0x66);
    insnbuf.push_back(0x90);

    instruction->set_raw_bytes(insnbuf);
    instruction->set_lockPrefix(false);
    instruction->set_repeatPrefix(x86_repeat_none);

    SgAsmOperandList *operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    return instruction;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
makeByteValue(uint8_t val) {
    return buildValueX86Byte(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
makeWordValue(uint16_t val) {
    return buildValueX86Word(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
makeDWordValue(uint32_t val) {
    return buildValueX86DWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
makeQWordValue(uint64_t val) {
    return buildValueX86QWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmMemoryReferenceExpression*
makeMemoryReference(SgAsmExpression *addr, SgAsmExpression *segment, SgAsmType *type) {
    return buildMemoryReferenceExpression(addr, segment, type);
}

SgAsmMemoryReferenceExpression*
buildMemoryReferenceExpression(SgAsmExpression *addr, SgAsmExpression *segment, SgAsmType *type) {
    SgAsmMemoryReferenceExpression *r = new SgAsmMemoryReferenceExpression(addr);
    addr->set_parent(r);
    if (segment) {
        r->set_segment(segment);
        segment->set_parent(r);
    }
    if (type)
        r->set_type(type);
    return r;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAdd*
makeAdd(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddExpression(lhs, rhs);
}

SgAsmBinaryAdd*
buildAddExpression(SgAsmExpression *lhs, SgAsmExpression *rhs)
{
    SgAsmBinaryAdd *a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtract*
makeSubtract(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractExpression(lhs, rhs);
}

SgAsmBinarySubtract*
buildSubtractExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtract *a = new SgAsmBinarySubtract(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAddPreupdate*
makeAddPreupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddPreupdateExpression(lhs, rhs);
}

SgAsmBinaryAddPreupdate*
buildAddPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAddPreupdate *a = new SgAsmBinaryAddPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtractPreupdate*
makeSubtractPreupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractPreupdateExpression(lhs, rhs);
}

SgAsmBinarySubtractPreupdate*
buildSubtractPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtractPreupdate *a = new SgAsmBinarySubtractPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAddPostupdate*
makeAddPostupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddPostupdateExpression(lhs, rhs);
}

SgAsmBinaryAddPostupdate*
buildAddPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAddPostupdate *a = new SgAsmBinaryAddPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtractPostupdate*
makeSubtractPostupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractPostupdateExpression(lhs, rhs);
}

SgAsmBinarySubtractPostupdate*
buildSubtractPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtractPostupdate *a = new SgAsmBinarySubtractPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryMultiply*
makeMul(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildMultiplyExpression(lhs, rhs);
}

SgAsmBinaryMultiply*
buildMultiplyExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryMultiply *a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryLsl*
makeLsl(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildLslExpression(lhs, rhs);
}

SgAsmBinaryLsl*
buildLslExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryLsl *a = new SgAsmBinaryLsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryLsr*
makeLsr(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildLsrExpression(lhs, rhs);
}

SgAsmBinaryLsr*
buildLsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryLsr *a = new SgAsmBinaryLsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAsr*
makeAsr(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAsrExpression(lhs, rhs);
}

SgAsmBinaryAsr*
buildAsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAsr *a = new SgAsmBinaryAsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryRor*
makeRor(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildRorExpression(lhs, rhs);
}

SgAsmBinaryRor*
buildRorExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryRor *a = new SgAsmBinaryRor(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmUnaryRrx*
makeRrx(SgAsmExpression *lhs) {
    return buildRrxExpression(lhs);
}

SgAsmUnaryRrx*
buildRrxExpression(SgAsmExpression *lhs) {
    SgAsmUnaryRrx *a = new SgAsmUnaryRrx(lhs);
    lhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmUnaryArmSpecialRegisterList*
makeArmSpecialRegisterList(SgAsmExpression *lhs) {
    return buildArmSpecialRegisterList(lhs);
}

SgAsmUnaryArmSpecialRegisterList*
buildArmSpecialRegisterList(SgAsmExpression *lhs) {
    SgAsmUnaryArmSpecialRegisterList *a = new SgAsmUnaryArmSpecialRegisterList(lhs);
    lhs->set_parent(a);
    return a;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op) {
    SgAsmRiscOperation *retval = new SgAsmRiscOperation(op);
    SgAsmExprListExp *operands = new SgAsmExprListExp;
    operands->set_parent(retval);
    retval->set_operands(operands);
    return retval;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op, SgAsmExpression *e1) {
    SgAsmRiscOperation *retval = buildRiscOperation(op);
    appendExpression(retval->get_operands(), e1);
    return retval;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op, SgAsmExpression *e1, SgAsmExpression *e2) {
    SgAsmRiscOperation *retval = buildRiscOperation(op);
    appendExpression(retval->get_operands(), e1);
    appendExpression(retval->get_operands(), e2);
    return retval;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op, SgAsmExpression *e1, SgAsmExpression *e2, SgAsmExpression *e3) {
    SgAsmRiscOperation *retval = buildRiscOperation(op);
    appendExpression(retval->get_operands(), e1);
    appendExpression(retval->get_operands(), e2);
    appendExpression(retval->get_operands(), e3);
    return retval;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op, SgAsmExpression *e1, SgAsmExpression *e2, SgAsmExpression *e3,
                   SgAsmExpression *e4) {
    SgAsmRiscOperation *retval = buildRiscOperation(op);
    appendExpression(retval->get_operands(), e1);
    appendExpression(retval->get_operands(), e2);
    appendExpression(retval->get_operands(), e3);
    appendExpression(retval->get_operands(), e4);
    return retval;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmExprListExp*
makeExprListExp() {
    return buildExprListExpression();
}

SgAsmExprListExp*
buildExprListExpression() {
    return new SgAsmExprListExp();
}

void
appendExpression(SgAsmExprListExp *exprList, SgAsmExpression *expr) {
    ASSERT_not_null(exprList);
    ASSERT_not_null(expr);
    expr->set_parent(exprList);
    exprList->get_expressions().push_back(expr);
}

SgAsmBlock*
buildBasicBlock(const std::vector<SgAsmInstruction*> &insns) {
    SgAsmBlock *bb = new SgAsmBlock;
    if (!insns.empty()) {
        bb->set_id(insns.front()->get_address());
        bb->set_address(insns.front()->get_address());
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            bb->get_statementList().push_back(insn);
            insn->set_parent(bb);
        }
    }
    return bb;
}

SgAsmStaticData*
buildStaticData(rose_addr_t startVa, const SgUnsignedCharList &rawData) {
    SgAsmStaticData *sd = new SgAsmStaticData;
    sd->set_address(startVa);
    sd->set_raw_bytes(rawData);
    return sd;
}

SgAsmBlock*
buildDataBlock(SgAsmStaticData *staticData) {
    ASSERT_not_null(staticData);
    SgAsmBlock *db = new SgAsmBlock;
    db->set_id(staticData->get_address());
    db->set_address(staticData->get_address());
    db->get_statementList().push_back(staticData);
    staticData->set_parent(db);
    return db;
}

SgAsmFunction*
buildFunction(rose_addr_t entryVa, const std::vector<SgAsmBlock*> &blocks) {
    SgAsmFunction *func = new SgAsmFunction;
    func->set_entry_va(entryVa);
    if (!blocks.empty()) {
        func->set_address(blocks.front()->get_address());
        BOOST_FOREACH (SgAsmBlock *block, blocks) {
            func->get_statementList().push_back(block);
            block->set_parent(func);
        }
    }
    return func;
}

} // namespace
} // namespace
