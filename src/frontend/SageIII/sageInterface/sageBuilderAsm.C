// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"

// DQ (5/1/2010): This is required to support the function: SgAsmInstruction* SageBuilderAsm::buildMultibyteNopInstruction(int n)
#include "InstructionEnumsX86.h"


using namespace std;

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
SageBuilderAsm::buildSgAsmx86RegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
SageBuilderAsm::buildSgAsmArmRegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
SageBuilderAsm::buildSgAsmMipsRegisterReferenceExpression(const RegisterDescriptor &desc) {
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmDirectRegisterExpression *
SageBuilderAsm::buildSgAsmPowerpcRegisterReferenceExpression(const RegisterDescriptor &desc)
{
    return new SgAsmDirectRegisterExpression(desc);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmx86Instruction*
SageBuilderAsm::buildx86Instruction(X86InstructionKind kind) {
    return buildX86Instruction(kind);
}

SgAsmx86Instruction*
SageBuilderAsm::buildX86Instruction(X86InstructionKind kind) {
    // These are the default values used for the construction of new instructions.
    rose_addr_t address = 0;
    std::string mnemonic = "";
    X86InstructionSize baseSize  = x86_insnsize_none;
    X86InstructionSize operandSize = x86_insnsize_none; 
    X86InstructionSize addressSize = x86_insnsize_none;

    SgAsmx86Instruction* instruction = new SgAsmx86Instruction(address, mnemonic, kind, baseSize, operandSize, addressSize);

    // This should not have been set yet.
    ASSERT_require(instruction->get_operandList() == NULL);

    // All instructions are required to have a valid SgAsmOperandList pointer.
    instruction->set_operandList(new SgAsmOperandList);
    instruction->get_operandList()->set_parent(instruction);

    return instruction;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmx86Instruction*
SageBuilderAsm::buildx86Instruction(X86InstructionKind kind, SgAsmExpression *operand) {
    return buildX86Instruction(kind, operand);
}

SgAsmx86Instruction*
SageBuilderAsm::buildX86Instruction(X86InstructionKind kind, SgAsmExpression *operand)
{
    SgAsmx86Instruction* instruction = buildX86Instruction(kind);
    appendOperand(instruction,operand);
    return instruction;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmx86Instruction*
SageBuilderAsm::buildx86Instruction(X86InstructionKind kind, SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildX86Instruction(kind, lhs, rhs);
}

SgAsmx86Instruction*
SageBuilderAsm::buildX86Instruction(X86InstructionKind kind, SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmx86Instruction* instruction = buildX86Instruction(kind);
    appendOperand(instruction,lhs);
    appendOperand(instruction,rhs);
    return instruction;
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU1(bool x) {
    return new SgAsmIntegerValueExpression(x, 1, buildTypeU1());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU8(uint8_t x) {
    return new SgAsmIntegerValueExpression(x, 8, buildTypeU8());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU16le(uint16_t x) {
    return new SgAsmIntegerValueExpression(x, 16, buildTypeU16le());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU16be(uint16_t x) {
    return new SgAsmIntegerValueExpression(x, 16, buildTypeU16be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU32le(uint32_t x) {
    return new SgAsmIntegerValueExpression(x, 32, buildTypeU32le());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU32be(uint32_t x) {
    return new SgAsmIntegerValueExpression(x, 32, buildTypeU32be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU64le(uint64_t x) {
    return new SgAsmIntegerValueExpression(x, 64, buildTypeU64be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueU64be(uint64_t x) {
    return new SgAsmIntegerValueExpression(x, 64, buildTypeU64be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI8(int8_t x) {
    return new SgAsmIntegerValueExpression((int64_t)x, 8, buildTypeI8());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI16le(int16_t x) {
    return new SgAsmIntegerValueExpression((int64_t)x, 16, buildTypeI16le());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI16be(int16_t x) {
    return new SgAsmIntegerValueExpression((int64_t)x, 16, buildTypeI16be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI32le(int32_t x) {
    return new SgAsmIntegerValueExpression((int64_t)x, 32, buildTypeI32le());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI32be(int32_t x) {
    return new SgAsmIntegerValueExpression((int64_t)x, 32, buildTypeI32be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI64le(int64_t x) {
    return new SgAsmIntegerValueExpression(x, 64, buildTypeI64be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueI64be(int64_t x) {
    return new SgAsmIntegerValueExpression(x, 64, buildTypeI64be());
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueIeee754Binary32le(double x) {
    return new SgAsmFloatValueExpression(x, buildIeee754Binary32le());
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueIeee754Binary32be(double x) {
    return new SgAsmFloatValueExpression(x, buildIeee754Binary32be());
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueIeee754Binary64le(double x) {
    return new SgAsmFloatValueExpression(x, buildIeee754Binary64le());
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueIeee754Binary64be(double x) {
    return new SgAsmFloatValueExpression(x, buildIeee754Binary64be());
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueX86Byte(uint8_t x) {
    return buildValueU8(x);
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueX86Word(uint16_t x) {
    return buildValueU16le(x);
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueX86DWord(uint32_t x) {
    return buildValueU32le(x);
}

SgAsmIntegerValueExpression*
SageBuilderAsm::buildValueX86QWord(uint64_t x) {
    return buildValueU64le(x);
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueX86Float32(double x) {
    return buildValueIeee754Binary32le(x);
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueX86Float64(double x) {
    return buildValueIeee754Binary64le(x);
}

SgAsmFloatValueExpression*
SageBuilderAsm::buildValueX86Float80(double x) {
    return new SgAsmFloatValueExpression(x, buildTypeX86Float80());
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::buildAsmDWordValue(uint32_t val) {
    return buildValueX86DWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::buildAsmByteValue(uint8_t val) {
    return buildValueX86Byte(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::buildAsmWordValue(uint16_t val) {
    return buildValueX86Word(val);
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU1() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 1, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU8() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 8, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU16le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 16, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU16be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 16, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU32le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 32, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU32be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 32, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU64le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 64, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeU64be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 64, false /*unsigned*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI8() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_UNSPECIFIED, 8, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI16le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 16, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI16be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 16, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI32le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 32, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI32be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 32, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI64le() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, 64, true /*signed*/));
    return cached;
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeI64be() {
    static SgAsmIntegerType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_MSB, 64, true /*signed*/));
    return cached;
}

SgAsmFloatType*
SageBuilderAsm::buildIeee754Binary32le() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmFloatType(ByteOrder::ORDER_LSB, 32, 0, 23, 31, 23, 8, 127));
    return cached;
}

SgAsmFloatType*
SageBuilderAsm::buildIeee754Binary32be() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmFloatType(ByteOrder::ORDER_MSB, 32, 0, 23, 31, 23, 8, 127));
    return cached;
}

SgAsmFloatType*
SageBuilderAsm::buildIeee754Binary64le() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmFloatType(ByteOrder::ORDER_LSB, 64, 0, 52, 63, 52, 11, 1023));
    return cached;
}

SgAsmFloatType*
SageBuilderAsm::buildIeee754Binary64be() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmFloatType(ByteOrder::ORDER_MSB, 64, 0, 52, 63, 52, 11, 1023));
    return cached;
}

SgAsmFloatType*
SageBuilderAsm::buildIeee754Binary80le() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = SgAsmType::registerOrDelete(new SgAsmFloatType(ByteOrder::ORDER_LSB, 80, 0, 64, 79, 64, 15, 16383));
    return cached;
}

SgAsmVectorType*
SageBuilderAsm::buildTypeVector(size_t nElmts, SgAsmType *elmtType) {
    return SgAsmType::registerOrDelete(new SgAsmVectorType(nElmts, elmtType));
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeX86Byte() {
    return buildTypeU8();
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeX86Word() {
    return buildTypeU16le();
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeX86DoubleWord() {
    return buildTypeU32le();
}

SgAsmIntegerType*
SageBuilderAsm::buildTypeX86QuadWord() {
    return buildTypeU64le();
}

SgAsmFloatType*
SageBuilderAsm::buildTypeX86Float32() {
    return buildIeee754Binary32le();
}

SgAsmFloatType*
SageBuilderAsm::buildTypeX86Float64() {
    return buildIeee754Binary64le();
}

SgAsmFloatType*
SageBuilderAsm::buildTypeX86Float80() {
    return buildIeee754Binary80le();
}

// M68k 96-bit "extended-precision real format"
// The 16 bits at [64-79] are always zero.
SgAsmFloatType*
SageBuilderAsm::buildTypeM68kFloat96() {
    static SgAsmFloatType *cached = NULL;
    if (!cached)
        cached = new SgAsmFloatType(ByteOrder::ORDER_MSB, 96, 0, 64, 95, 80, 15, 16383);
    return cached;
}

SgAsmVectorType*
SageBuilderAsm::buildTypeX86DoubleQuadWord() {
    SgAsmType *quadword = buildTypeX86QuadWord();
    return SgAsmType::registerOrDelete(new SgAsmVectorType(2, quadword));
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerType*
SageBuilderAsm::buildAsmTypeByte() {
    return buildTypeX86Byte();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
SageBuilderAsm::buildAsmTypeSingleFloat() {
    return buildTypeX86Float32();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
SageBuilderAsm::buildAsmTypeDoubleFloat() {
    return buildTypeX86Float64();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmFloatType*
SageBuilderAsm::buildAsmType80bitFloat() {
    return buildTypeX86Float80();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmVectorType*
SageBuilderAsm::buildAsmTypeDoubleQuadWord() {
    return buildTypeX86DoubleQuadWord();
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmInstruction*
SageBuilderAsm::buildMultibyteNopInstruction(int n) {
    return buildX86MultibyteNopInstruction(n);
}

SgAsmx86Instruction*
SageBuilderAsm::buildX86MultibyteNopInstruction(size_t nBytes) {
    ASSERT_require(nBytes > 0);
    ASSERT_require(nBytes <= 9);

    SgAsmx86Instruction *instruction = new SgAsmx86Instruction(0, "nop", x86_nop,
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
SageBuilderAsm::makeByteValue(uint8_t val) {
    return buildValueX86Byte(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::makeWordValue(uint16_t val) {
    return buildValueX86Word(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::makeDWordValue(uint32_t val) {
    return buildValueX86DWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmIntegerValueExpression*
SageBuilderAsm::makeQWordValue(uint64_t val) {
    return buildValueX86QWord(val);
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmMemoryReferenceExpression*
SageBuilderAsm::makeMemoryReference(SgAsmExpression *addr, SgAsmExpression *segment, SgAsmType *type) {
    return buildMemoryReferenceExpression(addr, segment, type);
}

SgAsmMemoryReferenceExpression*
SageBuilderAsm::buildMemoryReferenceExpression(SgAsmExpression *addr, SgAsmExpression *segment, SgAsmType *type) {
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
SageBuilderAsm::makeAdd(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddExpression(lhs, rhs);
}

SgAsmBinaryAdd*
SageBuilderAsm::buildAddExpression(SgAsmExpression *lhs, SgAsmExpression *rhs)
{
    SgAsmBinaryAdd *a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtract*
SageBuilderAsm::makeSubtract(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractExpression(lhs, rhs);
}

SgAsmBinarySubtract*
SageBuilderAsm::buildSubtractExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtract *a = new SgAsmBinarySubtract(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAddPreupdate*
SageBuilderAsm::makeAddPreupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddPreupdateExpression(lhs, rhs);
}

SgAsmBinaryAddPreupdate*
SageBuilderAsm::buildAddPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAddPreupdate *a = new SgAsmBinaryAddPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtractPreupdate*
SageBuilderAsm::makeSubtractPreupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractPreupdateExpression(lhs, rhs);
}

SgAsmBinarySubtractPreupdate*
SageBuilderAsm::buildSubtractPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtractPreupdate *a = new SgAsmBinarySubtractPreupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAddPostupdate*
SageBuilderAsm::makeAddPostupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAddPostupdateExpression(lhs, rhs);
}

SgAsmBinaryAddPostupdate*
SageBuilderAsm::buildAddPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAddPostupdate *a = new SgAsmBinaryAddPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinarySubtractPostupdate*
SageBuilderAsm::makeSubtractPostupdate(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildSubtractPostupdateExpression(lhs, rhs);
}

SgAsmBinarySubtractPostupdate*
SageBuilderAsm::buildSubtractPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinarySubtractPostupdate *a = new SgAsmBinarySubtractPostupdate(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryMultiply*
SageBuilderAsm::makeMul(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildMultiplyExpression(lhs, rhs);
}

SgAsmBinaryMultiply*
SageBuilderAsm::buildMultiplyExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryMultiply *a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryLsl*
SageBuilderAsm::makeLsl(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildLslExpression(lhs, rhs);
}

SgAsmBinaryLsl*
SageBuilderAsm::buildLslExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryLsl *a = new SgAsmBinaryLsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryLsr*
SageBuilderAsm::makeLsr(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildLsrExpression(lhs, rhs);
}

SgAsmBinaryLsr*
SageBuilderAsm::buildLsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryLsr *a = new SgAsmBinaryLsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryAsr*
SageBuilderAsm::makeAsr(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildAsrExpression(lhs, rhs);
}

SgAsmBinaryAsr*
SageBuilderAsm::buildAsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryAsr *a = new SgAsmBinaryAsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmBinaryRor*
SageBuilderAsm::makeRor(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    return buildRorExpression(lhs, rhs);
}

SgAsmBinaryRor*
SageBuilderAsm::buildRorExpression(SgAsmExpression *lhs, SgAsmExpression *rhs) {
    SgAsmBinaryRor *a = new SgAsmBinaryRor(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmUnaryRrx*
SageBuilderAsm::makeRrx(SgAsmExpression *lhs) {
    return buildRrxExpression(lhs);
}

SgAsmUnaryRrx*
SageBuilderAsm::buildRrxExpression(SgAsmExpression *lhs) {
    SgAsmUnaryRrx *a = new SgAsmUnaryRrx(lhs);
    lhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmUnaryArmSpecialRegisterList*
SageBuilderAsm::makeArmSpecialRegisterList(SgAsmExpression *lhs) {
    return buildArmSpecialRegisterList(lhs);
}

SgAsmUnaryArmSpecialRegisterList*
SageBuilderAsm::buildArmSpecialRegisterList(SgAsmExpression *lhs) {
    SgAsmUnaryArmSpecialRegisterList *a = new SgAsmUnaryArmSpecialRegisterList(lhs);
    lhs->set_parent(a);
    return a;
}

// FIXME[Robb P. Matzke 2014-07-21]: deprecated
SgAsmExprListExp*
SageBuilderAsm::makeExprListExp() {
    return buildExprListExpression();
}

SgAsmExprListExp*
SageBuilderAsm::buildExprListExpression() {
    return new SgAsmExprListExp();
}
