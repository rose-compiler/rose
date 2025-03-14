#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include "SageBuilderAsm.h"

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/BinaryAnalysis/BinaryLoader.h>

#include <boost/foreach.hpp>
#include <Sawyer/CommandLine.h>                         // needed for CommandLine::Parser.programName(), i.e., argv[0]

using namespace Rose::BinaryAnalysis;

namespace Rose {
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

     // DQ (11/25/2020): Add support to set this as a binary file (there is at least one binary file processed by ROSE).
        Rose::is_binary_executable = true;
    } else {
        // No project yet, so just call frontend()
        project = frontend(args);
        retval = SageInterface::querySubTree<SgBinaryComposite>(project).front();
        ASSERT_not_null(retval);
    }
    return retval;
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

SgAsmIntegerType*
buildTypeU(size_t nBits) {
    typedef boost::unordered_map<size_t, SgAsmIntegerType*> Cache;
    static Cache cache;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    SgAsmIntegerType *retval = NULL;
    Cache::iterator found = cache.find(nBits);
    if (found == cache.end()) {
        ByteOrder::Endianness sex = nBits <= 8 ? ByteOrder::ORDER_UNSPECIFIED : ByteOrder::ORDER_LSB;
        retval = SgAsmType::registerOrDelete(new SgAsmIntegerType(sex, nBits, false /*unsigned*/));
        cache.insert(std::make_pair(nBits, retval));
    } else {
        retval = found->second;
    }
    ASSERT_not_null(retval);
    return retval;
}

SgAsmIntegerType*
buildTypeI(size_t nBits) {
    if (1 == nBits)
        return buildTypeU(1);

    typedef boost::unordered_map<size_t, SgAsmIntegerType*> Cache;
    static Cache cache;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    SgAsmIntegerType *retval = NULL;
    Cache::iterator found = cache.find(nBits);
    if (found == cache.end()) {
        ByteOrder::Endianness sex = nBits <= 8 ? ByteOrder::ORDER_UNSPECIFIED : ByteOrder::ORDER_LSB;
        retval = SgAsmType::registerOrDelete(new SgAsmIntegerType(sex, nBits, true /*signed*/));
        cache.insert(std::make_pair(nBits, retval));
    } else {
        retval = found->second;
    }
    ASSERT_not_null(retval);
    return retval;
}

SgAsmIntegerType*
buildTypeU1() {
    return buildTypeU(1);
}

SgAsmIntegerType*
buildTypeU4() {
    return buildTypeU(4);
}

SgAsmIntegerType*
buildTypeU8() {
    return buildTypeU(8);
}

SgAsmIntegerType*
buildTypeU16() {
    return buildTypeU(16);
}

SgAsmIntegerType*
buildTypeU32() {
    return buildTypeU(32);
}

SgAsmIntegerType*
buildTypeU64() {
    return buildTypeU(64);
}

SgAsmIntegerType*
buildTypeI8() {
    return buildTypeI(8);
}

SgAsmIntegerType*
buildTypeI16() {
    return buildTypeI(16);
}

SgAsmIntegerType*
buildTypeI32() {
    return buildTypeI(32);
}

SgAsmIntegerType*
buildTypeI64() {
    return buildTypeI(64);
}

SgAsmFloatType*
buildIeee754Binary16() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 16,
                                                    SgAsmFloatType::BitRange::baseSize(0, 11), // significand
                                                    SgAsmFloatType::BitRange::baseSize(11, 5), // exponent
                                                    16,                                        // sign bit
                                                    15,                                        // exponent bias
                                                    SgAsmFloatType::ieeeFlags());
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmFloatType*
buildIeee754Binary32() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 32,
                                                    SgAsmFloatType::BitRange::baseSize(0, 23), // significand
                                                    SgAsmFloatType::BitRange::baseSize(23, 8), // exponent
                                                    31,                                        // sign bit
                                                    127,                                       // exponent bias
                                                    SgAsmFloatType::ieeeFlags());
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmFloatType*
buildIeee754Binary64() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 64,
                                                    SgAsmFloatType::BitRange::baseSize(0, 52),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(52, 11), // exponent
                                                    63,                                         // sign bit
                                                    1023,                                       // exponent bias
                                                    SgAsmFloatType::ieeeFlags());
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmFloatType*
buildIeee754Binary128() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 128,
                                                    SgAsmFloatType::BitRange::baseSize(0, 112),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(112, 15), // exponent
                                                    127,                                         // sign bit
                                                    262143,                                      // exponent bias
                                                    SgAsmFloatType::ieeeFlags());
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
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 80,
                                                    SgAsmFloatType::BitRange::baseSize(0, 64),  // significand
                                                    SgAsmFloatType::BitRange::baseSize(64, 15), // exponent
                                                    79,                                         // sign bit
                                                    16383,                                      // exponent bias
                                                    SgAsmFloatType::ieeeFlags()
                                                        .clear(SgAsmFloatType::IMPLICIT_BIT_CONVENTION));
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

// M68k 96-bit "extended-precision real format"
// The 16 bits at [64-79] are always zero.
SgAsmFloatType*
buildTypeM68kFloat96() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        // From the Motorola 68000 documentation, "For all three [binary floating-point] precisions, a normalized mantissa is
        // always in the range (1.0...2.0). The extended-precision data format represents the entire mantissa, including the
        // explicit integer part bit. Single- and double-precision data formats represent only a fractional portion of the
        // mantissa (the fraction) and always imply the integer part as one". The documentation states that for
        // extended-precision types, the term "matissa" is equivalent to IEEE 754 "significand" since the integer part of the
        // significand is always stored.
        //
        // A table subsequently describes that the significand is denormalized when the exponent field's bits are all clear,
        // implying that gradual underflow capability is present.
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 96,
                                                    SgAsmFloatType::BitRange::baseSize(0, 64),  // significand w/explicit integer bit
                                                    SgAsmFloatType::BitRange::baseSize(80, 15), // exponent
                                                    95,                                         // sign bit
                                                    0x3fff,                                     // exponent bias
                                                    SgAsmFloatType::ieeeFlags()
                                                        .clear(SgAsmFloatType::IMPLICIT_BIT_CONVENTION));
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

// M68k 80-bit floating-point type. Same as the 96 bit "extended-precision real" type but without the 16 bits field that's
// always zero. This is the type actually stored in the floating-point registers for Motorola (but not ColdFire) processors.
SgAsmFloatType*
buildTypeM68kFloat80() {
    static SgAsmFloatType *cached = NULL;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!cached) {
        SgAsmFloatType *fpType = new SgAsmFloatType(ByteOrder::ORDER_LSB, 80,
                                                    SgAsmFloatType::BitRange::baseSize(0, 64),  // significand w/explicit integer bit
                                                    SgAsmFloatType::BitRange::baseSize(64, 15), // exponent
                                                    79,                                         // sign bit
                                                    0x3fff,                                     // exponent bias
                                                    SgAsmFloatType::ieeeFlags()
                                                        .clear(SgAsmFloatType::IMPLICIT_BIT_CONVENTION));
        cached = SgAsmType::registerOrDelete(fpType);
    }
    return cached;
}

SgAsmVectorType*
buildTypeX86DoubleQuadWord() {
    SgAsmType *quadword = buildTypeX86QuadWord();
    return SgAsmType::registerOrDelete(new SgAsmVectorType(2, quadword));
}

SgAsmX86Instruction*
buildX86MultibyteNopInstruction(size_t nBytes) {
    ASSERT_require(nBytes > 0);
    ASSERT_require(nBytes <= 9);

    auto arch = Architecture::findByName("intel-pentium4").orThrow();
    SgAsmX86Instruction *instruction = new SgAsmX86Instruction(0, *arch->registrationId(), x86_nop,
                                                               x86_insnsize_32, x86_insnsize_32, x86_insnsize_32);

    // Build a simple version of multi-byte nop using repeated prefixes.
    SgUnsignedCharList insnbuf;
    for (size_t i = 1; i < nBytes; i++)
        insnbuf.push_back(0x66);
    insnbuf.push_back(0x90);

    instruction->set_rawBytes(insnbuf);
    instruction->set_lockPrefix(false);
    instruction->set_repeatPrefix(x86_repeat_none);

    SgAsmOperandList *operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    return instruction;
}

SgAsmMemoryReferenceExpression*
buildMemoryReferenceExpression(SgAsmExpression *addr, SgAsmExpression *segment, SgAsmType *type) {
    SgAsmMemoryReferenceExpression *r = new SgAsmMemoryReferenceExpression(addr, segment);
    addr->set_parent(r);
    if (segment)
        segment->set_parent(r);
    if (type)
        r->set_type(type);
    return r;
}

SgAsmBinaryAdd*
buildAddExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryAdd *a = new SgAsmBinaryAdd(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinarySubtract*
buildSubtractExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinarySubtract *a = new SgAsmBinarySubtract(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryPreupdate*
buildPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    ASSERT_not_null(lhs);
    ASSERT_not_null(rhs);
    ASSERT_forbid(lhs->get_parent());
    ASSERT_forbid(rhs->get_parent());
    SgAsmBinaryPreupdate *retval = new SgAsmBinaryPreupdate(lhs, rhs);
    lhs->set_parent(retval);
    rhs->set_parent(retval);
    if (type) {
        retval->set_type(type);
    } else if (lhs->get_type()) {
        retval->set_type(lhs->get_type());
    } else {
        retval->set_type(rhs->get_type());
    }
    return retval;
}

SgAsmBinaryPostupdate*
buildPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    ASSERT_not_null(lhs);
    ASSERT_not_null(rhs);
    ASSERT_forbid(lhs->get_parent());
    ASSERT_forbid(rhs->get_parent());
    SgAsmBinaryPostupdate *retval = new SgAsmBinaryPostupdate(lhs, rhs);
    lhs->set_parent(retval);
    rhs->set_parent(retval);
    if (type) {
        retval->set_type(type);
    } else if (lhs->get_type()) {
        retval->set_type(lhs->get_type());
    } else {
        retval->set_type(rhs->get_type());
    }
    return retval;
}

SgAsmBinaryMultiply*
buildMultiplyExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryMultiply *a = new SgAsmBinaryMultiply(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryLsl*
buildLslExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryLsl *a = new SgAsmBinaryLsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryMsl*
buildMslExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryMsl *a = new SgAsmBinaryMsl(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryLsr*
buildLsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryLsr *a = new SgAsmBinaryLsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryAsr*
buildAsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryAsr *a = new SgAsmBinaryAsr(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryRor*
buildRorExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type) {
    SgAsmBinaryRor *a = new SgAsmBinaryRor(lhs, rhs);
    lhs->set_parent(a);
    rhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else if (lhs->get_type()) {
        a->set_type(lhs->get_type());
    } else {
        a->set_type(rhs->get_type());
    }
    return a;
}

SgAsmBinaryConcat*
buildConcatExpression(SgAsmExpression *msb, SgAsmExpression *lsb) {
    ASSERT_not_null(msb);
    ASSERT_not_null(lsb);
    ASSERT_not_null(msb->get_type());
    ASSERT_not_null(lsb->get_type());
    ASSERT_forbid(msb->get_parent());
    ASSERT_forbid(lsb->get_parent());

    SgAsmBinaryConcat *a = new SgAsmBinaryConcat(msb, lsb);
    msb->set_parent(a);
    lsb->set_parent(a);
    a->set_type(buildTypeU(msb->get_type()->get_nBits() + lsb->get_type()->get_nBits()));
    return a;
}

SgAsmUnaryRrx*
buildRrxExpression(SgAsmExpression *lhs, SgAsmType *type) {
    SgAsmUnaryRrx *a = new SgAsmUnaryRrx(lhs);
    lhs->set_parent(a);
    if (type) {
        a->set_type(type);
    } else {
        a->set_type(lhs->get_type());
    }
    return a;
}

SgAsmUnaryTruncate*
buildTruncateExpression(SgAsmExpression *arg, SgAsmType *type) {
    ASSERT_not_null(arg);
    ASSERT_not_null(type);
    SgAsmUnaryTruncate *retval = new SgAsmUnaryTruncate(arg);
    arg->set_parent(retval);
    retval->set_type(type);
    return retval;
}

SgAsmUnarySignedExtend*
buildSignedExtendExpression(SgAsmExpression *arg, SgAsmType *type) {
    ASSERT_not_null(arg);
    ASSERT_not_null(type);
    SgAsmUnarySignedExtend *retval = new SgAsmUnarySignedExtend(arg);
    arg->set_parent(retval);
    retval->set_type(type);
    return retval;
}

SgAsmUnaryUnsignedExtend*
buildUnsignedExtendExpression(SgAsmExpression *arg, SgAsmType *type) {
    ASSERT_not_null(arg);
    ASSERT_not_null(type);
    SgAsmUnaryUnsignedExtend *retval = new SgAsmUnaryUnsignedExtend(arg);
    arg->set_parent(retval);
    retval->set_type(type);
    return retval;
}

SgAsmRiscOperation*
buildRiscOperation(SgAsmRiscOperation::RiscOperator op) {
    SgAsmRiscOperation *retval = new SgAsmRiscOperation(op);
    SgAsmExprListExp *operands = new SgAsmExprListExp;
    retval->set_operands(operands);
    operands->set_parent(retval);
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

SgAsmExprListExp*
buildExprListExpression() {
    return new SgAsmExprListExp();
}

void
appendExpression(SgAsmExprListExp *exprList, SgAsmExpression *expr) {
    ASSERT_not_null(exprList);
    ASSERT_not_null(expr);
    exprList->get_expressions().push_back(expr);
    expr->set_parent(exprList);
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
buildStaticData(Rose::BinaryAnalysis::Address startVa, const SgUnsignedCharList &rawData) {
    SgAsmStaticData *sd = new SgAsmStaticData;
    sd->set_address(startVa);
    sd->set_rawBytes(rawData);
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
buildFunction(Rose::BinaryAnalysis::Address entryVa, const std::vector<SgAsmBlock*> &blocks) {
    SgAsmFunction *func = new SgAsmFunction;
    func->set_entryVa(entryVa);
    if (!blocks.empty()) {
        func->set_address(blocks.front()->get_address());
        BOOST_FOREACH (SgAsmBlock *block, blocks) {
            func->get_statementList().push_back(block);
            block->set_parent(func);
        }
    }
    return func;
}

// Template specializations for building integer values
template<> SgAsmIntegerValueExpression* buildValue<bool    >(bool     x) {return buildValueU1(x);}
template<> SgAsmIntegerValueExpression* buildValue<uint8_t >(uint8_t  x) {return buildValueU8(x);}
template<> SgAsmIntegerValueExpression* buildValue<uint16_t>(uint16_t x) {return buildValueU16(x);}
template<> SgAsmIntegerValueExpression* buildValue<uint32_t>(uint32_t x) {return buildValueU32(x);}
template<> SgAsmIntegerValueExpression* buildValue<uint64_t>(uint64_t x) {return buildValueU64(x);}

template<> SgAsmIntegerValueExpression* buildValue<int8_t >(int8_t  x) {return buildValueI8(x);}
template<> SgAsmIntegerValueExpression* buildValue<int16_t>(int16_t x) {return buildValueI16(x);}
template<> SgAsmIntegerValueExpression* buildValue<int32_t>(int32_t x) {return buildValueI32(x);}
template<> SgAsmIntegerValueExpression* buildValue<int64_t>(int64_t x) {return buildValueI64(x);}

} // namespace
} // namespace

#endif
