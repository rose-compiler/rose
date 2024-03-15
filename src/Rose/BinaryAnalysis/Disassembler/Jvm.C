#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmIntegerValueExpression.h>
#include <SgAsmJvmInstruction.h>

#include <SageBuilderAsm.h>

#define DEBUG_ON 0

using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

using opcode = JvmInstructionKind;
using namespace ByteOrder;

rose_addr_t
Jvm::codeOffset() {
    return codeOffset_;
}

void
Jvm::codeOffset(rose_addr_t offset) {
    codeOffset_ = offset;
}

template <class T> size_t
Jvm::appendOperand(const MemoryMap::Ptr &map, rose_addr_t va,
                   SgUnsignedCharList &chars, SgAsmOperandList* operands)
{
  const size_t bufSize{sizeof(T)};
  uint8_t buf[bufSize];

  size_t nRead = map->at(va).limit(bufSize).require(MemoryMap::READABLE).read(buf).size();
  if (nRead != bufSize) {
#if TRIALS
    throw Exception("short read", va);
#endif
    ROSE_ASSERT(false && "short read");
  }

  unsigned char* cptr{buf}; // work around warning message: "dereferencing type-punned pointer"
  T val = beToHost(*reinterpret_cast<T*>(cptr));
  operands->appendOperand(SageBuilderAsm::buildValue(val));

  // TODO: Perhaps there is a more efficient way to do this
  for (size_t i = 0; i < nRead; i++) {
    chars.push_back(buf[i]);
  }

  return nRead;
}

size_t
Jvm::appendTableswitch(const MemoryMap::Ptr &map, rose_addr_t start,
                       SgUnsignedCharList &chars, SgAsmOperandList* operands)
{
  rose_addr_t va{start};

  // switch default must begin on 4 byte boundary, skip padding
  ASSERT_require(va >= codeOffset());
  const size_t nPad{(4 - (va - codeOffset())%4)%4};
  const size_t nBuf{nPad + 3*sizeof(int32_t)};
  uint8_t buf[nBuf];

  size_t nRead = map->at(va).limit(nBuf).require(MemoryMap::READABLE).read(buf).size();
  if (nRead != nBuf) {
    ASSERT_require2(false, "short read");
  }
  va += nRead;

  for (size_t i = 0; i < nRead; i++) {
    chars.push_back(buf[i]);
  }

  uint8_t* ptr{buf + nPad};
  int32_t def  = beToHost(*(int32_t*) ptr);  ptr += sizeof(int32_t);
  int32_t low  = beToHost(*(int32_t*) ptr);  ptr += sizeof(int32_t);
  int32_t high = beToHost(*(int32_t*) ptr);  ptr += sizeof(int32_t);

#if DEBUG_ON
  std::cout << "... appendTableswitch: "
            << def << ": "
            << low << ": "
            << high << ": "
            << std::endl;
#endif

  operands->appendOperand(SageBuilderAsm::buildValue(def));
  operands->appendOperand(SageBuilderAsm::buildValue(low));
  operands->appendOperand(SageBuilderAsm::buildValue(high));

  int nOff{high-low+1};
  for (int i = 0; i < nOff; i++) {
    auto count = appendOperand<int32_t>(map, va, chars, operands);
    nRead += count;
    va += count;
  }
  return nRead;
}

size_t
Jvm::appendLookupswitch(const MemoryMap::Ptr &map, rose_addr_t start,
                        SgUnsignedCharList &chars, SgAsmOperandList* operands)
{
  rose_addr_t va{start};

  // switch default must begin on 4 byte boundary, skip padding
  ASSERT_require(va >= codeOffset());
  const size_t nPad{(4 - (va - codeOffset())%4)%4};
  const size_t nBuf{nPad + 2*sizeof(int32_t)};
  uint8_t buf[nBuf];

  size_t nRead = map->at(va).limit(nBuf).require(MemoryMap::READABLE).read(buf).size();
  if (nRead != nBuf) {
    ASSERT_require2(false, "short read");
  }
  va += nRead;

  for (size_t i = 0; i < nRead; i++) {
    chars.push_back(buf[i]);
  }

  uint8_t* ptr{buf + nPad};
  int32_t def = beToHost(*(int32_t*) ptr);  ptr += sizeof(int32_t);
  int32_t nPairs = beToHost(*(int32_t*) ptr);  ptr += sizeof(int32_t);

#if DEBUG_ON
  std::cout << "... appendLookupswitch: "
            << def << ": "
            << nPairs << ": "
            << std::endl;
#endif

  operands->appendOperand(SageBuilderAsm::buildValue(def));
  operands->appendOperand(SageBuilderAsm::buildValue(nPairs));

  for (int i = 0; i < 2*nPairs; i++) {
    auto count = appendOperand<int32_t>(map, va, chars, operands);
    nRead += count;
    va += count;
  }
  return nRead;
}

Jvm::Jvm(const Architecture::Base::ConstPtr &arch)
    : Base(arch) {}

Jvm::Ptr
Jvm::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Jvm(arch));
}

Jvm::~Jvm() {}

Disassembler::Base::Ptr
Jvm::clone() const {
    return Ptr(new Jvm(architecture()));
}

SgAsmInstruction*
Jvm::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start, AddressSet*)
{
  SgAsmJvmInstruction* insn{nullptr};

  rose_addr_t va{start};
  uint8_t jbc = static_cast<uint8_t>(JvmInstructionKind::unknown);
  size_t nRead = map->at(va).limit(1).require(MemoryMap::READABLE).read(&jbc).size();
  if (0 == nRead) {
      throw Disassembler::Exception("short read", va);
  }
  va += 1; // advance to operands (if any)

  std::string mnemonic{};
  SgUnsignedCharList chars{jbc};
  auto operands = new SgAsmOperandList;
  opcode kind{static_cast<opcode>(jbc)};

  switch (kind) {
      case opcode::nop: // 0x00 (0)
        mnemonic = "nop";
        break;
      case opcode::aconst_null: // 0x01 (1)
        mnemonic = "aconst_null";
        break;
      case opcode::iconst_m1: // 0x02 (2)
        mnemonic = "iconst_m1";
        break;
      case opcode::iconst_0:  // 0x03 (3)
        mnemonic = "iconst_0";
        break;
      case opcode::iconst_1:  // 0x04 (4)
        mnemonic = "iconst_1";
        break;
      case opcode::iconst_2:  // 0x05 (5)
        mnemonic = "iconst_2";
        break;
      case opcode::iconst_3:  // 0x06 (6)
        mnemonic = "iconst_3";
        break;
      case opcode::iconst_4:  // 0x07 (7)
        mnemonic = "iconst_4";
        break;
      case opcode::iconst_5:  // 0x08 (8)
        mnemonic = "iconst_5";
        break;
      case opcode::lconst_0:  // 0x09 (9)
        mnemonic = "lconst_0";
        break;
      case opcode::lconst_1:  // 0x0a (10)
        mnemonic = "lconst_1";
        break;
      case opcode::fconst_0:  // 0x0b (11)
        mnemonic = "fconst_0";
        break;
      case opcode::fconst_1:  // 0x0c (12)
        mnemonic = "fconst_1";
        break;
      case opcode::fconst_2:  // 0x0d (13)
        mnemonic = "fconst_1";
        break;
      case opcode::dconst_0:  // 0x0e (14)
        mnemonic = "dconst_0";
        break;
      case opcode::dconst_1:  // 0x0f (15)
        mnemonic = "dconst_1";
        break;
      case opcode::bipush: // 0x10 (16)
        mnemonic = "bipush";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::sipush: // 0x11 (17)
        mnemonic = "sipush";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::ldc: // 0x12 (18)
        mnemonic = "ldc";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::ldc_w: // 0x13 (19)
        mnemonic = "ldc_w";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::ldc2_w: // 0x14 (20)
        mnemonic = "ldc2_w";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::iload: // 0x15 (21)
        mnemonic = "iload";
        va += appendOperand<uint8_t>(map, va, chars, operands); // index
        break;
      case opcode::lload: // 0x16 (22)
        mnemonic = "lload";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::fload: // 0x17 (23)
        mnemonic = "fload";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::dload: // 0x18 (24)
        mnemonic = "dload";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::aload: // 0x19 (25)
        mnemonic = "aload";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::iload_0: // 0x1a (26)
        mnemonic = "iload_0";
        break;
      case opcode::iload_1: // 0x1b (27)
        mnemonic = "iload_1";
        break;
      case opcode::iload_2: // 0x1c (28)
        mnemonic = "iload_2";
        break;
      case opcode::iload_3: // 0x1d (29)
        mnemonic = "iload_3";
        break;
      case opcode::lload_0: // 0x1e (30)
        mnemonic = "lload_0";
        break;
      case opcode::lload_1: // 0x1f (31)
        mnemonic = "lload_1";
        break;
      case opcode::lload_2: // 0x20 (32)
        mnemonic = "lload_2";
        break;
      case opcode::lload_3: // 0x21 (33)
        mnemonic = "lload_3";
        break;
      case opcode::fload_0: // 0x22 (34)
        mnemonic = "fload_0";
        break;
      case opcode::fload_1: // 0x23 (35)
        mnemonic = "fload_1";
        break;
      case opcode::fload_2: // 0x24 (36)
        mnemonic = "fload_2";
        break;
      case opcode::fload_3: // 0x25 (37)
        mnemonic = "fload_3";
        break;
      case opcode::dload_0: // 0x26 (38)
        mnemonic = "dload_0";
        break;
      case opcode::dload_1: // 0x27 (39)
        mnemonic = "dload_1";
        break;
      case opcode::dload_2: // 0x28 (40)
        mnemonic = "dload_2";
        break;
      case opcode::dload_3: // 0x29 (41)
        mnemonic = "dload_3";
        break;
      case opcode::aload_0: // 0x2a (42)
        mnemonic = "aload_0";
        break;
      case opcode::aload_1: // 0x2b (43)
        mnemonic = "aload_1";
        break;
      case opcode::aload_2: // 0x2c (44)
        mnemonic = "aload_2";
        break;
      case opcode::aload_3: // 0x2d (45)
        mnemonic = "aload_3";
        break;
      case opcode::iaload: // 0x2e (46)
        mnemonic = "iaload";
        break;
      case opcode::laload: // 0x2f (47)
        mnemonic = "laload";
        break;
      case opcode::faload: // 0x30 (48)
        mnemonic = "faload";
        break;
      case opcode::daload: // 0x31 (49)
        mnemonic = "daload";
        break;
      case opcode::aaload: // 0x32 (50)
        mnemonic = "aaload";
        break;
      case opcode::baload: // 0x33 (51)
        mnemonic = "baload";
        break;
      case opcode::caload: // 0x34 (52)
        mnemonic = "caload";
        break;
      case opcode::saload: // 0x35 (53)
        mnemonic = "saload";
        break;
      case opcode::istore: // 0x36 (54)
        mnemonic = "istore";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::lstore: // 0x37 (55)
        va += appendOperand<uint8_t>(map, va, chars, operands);
        mnemonic = "lstore";
        break;
      case opcode::fstore: // 0x38 (56)
        mnemonic = "fstore";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::dstore: // 0x39 (57)
        mnemonic = "dstore";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::astore: // 0x3a (58)
        mnemonic = "astore";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::istore_0: // 0x3b (59)
        mnemonic = "istore_0";
        break;
      case opcode::istore_1: // 0x3c (60)
        mnemonic = "istore_1";
        break;
      case opcode::istore_2: // 0x3d (61)
        mnemonic = "istore_2";
        break;
      case opcode::istore_3: // 0x3e (62)
        mnemonic = "istore_3";
        break;
      case opcode::lstore_0: // 0x3f (63)
        mnemonic = "lstore_0";
        break;
      case opcode::lstore_1: // 0x40 (64)
        mnemonic = "lstore_1";
        break;
      case opcode::lstore_2: // 0x41 (65)
        mnemonic = "lstore_2";
        break;
      case opcode::lstore_3: // 0x42 (66)
        mnemonic = "lstore_3";
        break;
      case opcode::fstore_0: // 0x43 (67)
        mnemonic = "fstore_0";
        break;
      case opcode::fstore_1: // 0x44 (68)
        mnemonic = "fstore_1";
        break;
      case opcode::fstore_2: // 0x45 (69)
        mnemonic = "fstore_2";
        break;
      case opcode::fstore_3: // 0x46 (70)
        mnemonic = "fstore_3";
        break;
      case opcode::dstore_0: // 0x47 (71)
        mnemonic = "dstore_0";
        break;
      case opcode::dstore_1: // 0x48 (72)
        mnemonic = "dstore_1";
        break;
      case opcode::dstore_2: // 0x49 (73)
        mnemonic = "dstore_2";
        break;
      case opcode::dstore_3: // 0x4a (74)
        mnemonic = "dstore_3";
        break;
      case opcode::astore_0: // 0x4b (75)
        mnemonic = "astore_0";
        break;
      case opcode::astore_1: // 0x4c (76)
        mnemonic = "astore_1";
        break;
      case opcode::astore_2: // 0x4d (77)
        mnemonic = "astore_2";
        break;
      case opcode::astore_3: // 0x4e (78)
        mnemonic = "astore_3";
        break;
      case opcode::iastore: // 0x4f (79)
        mnemonic = "iastore";
        break;
      case opcode::lastore: // 0x50 (80)
        mnemonic = "lastore";
        break;
      case opcode::fastore: // 0x51 (81)
        mnemonic = "fastore";
        break;
      case opcode::dastore: // 0x52 (82)
        mnemonic = "dastore";
        break;
      case opcode::aastore: // 0x53 (83)
        mnemonic = "aastore";
        break;
      case opcode::bastore: // 0x54 (84)
        mnemonic = "bastore";
        break;
      case opcode::castore: // 0x55 (85)
        mnemonic = "castore";
        break;
      case opcode::sastore: // 0x56 (86)
        mnemonic = "sastore";
        break;
      case opcode::pop: // 0x57 (87)
        mnemonic = "pop";
        break;
      case opcode::pop2: // 0x58 (88)
        mnemonic = "pop2";
        break;
      case opcode::dup: // 0x59 (89)
        mnemonic = "dup";
        break;
      case opcode::dup_x1: // 0x5a (90)
        mnemonic = "dup_x1";
        break;
      case opcode::dup_x2: // 0x5b (91)
        mnemonic = "dup_x2";
        break;
      case opcode::dup2: // 0x5c (92)
        mnemonic = "dup2";
        break;
      case opcode::dup2_x1: // 0x5d (93)
        mnemonic = "dup2_x1";
        break;
      case opcode::dup2_x2: // 0x5e (94)
        mnemonic = "dup2_x2";
        break;
      case opcode::swap: // 0x5f (95)
        mnemonic = "swap";
        break;
      case opcode::iadd: // 0x60 (96)
        mnemonic = "iadd";
        break;
      case opcode::ladd: // 0x61 (97)
        mnemonic = "ladd";
        break;
      case opcode::fadd: // 0x62 (98)
        mnemonic = "fadd";
        break;
      case opcode::dadd: // 0x63 (99)
        mnemonic = "dadd";
        break;
      case opcode::isub: // 0x64 (100)
        mnemonic = "isub";
        break;
      case opcode::lsub: // 0x65 (101)
        mnemonic = "lsub";
        break;
      case opcode::fsub: // 0x66 (102)
        mnemonic = "fsub";
        break;
      case opcode::dsub: // 0x67 (103)
        mnemonic = "dsub";
        break;
      case opcode::imul: // 0x68 (104)
        mnemonic = "imul";
        break;
      case opcode::lmul: // 0x69 (105)
        mnemonic = "lmul";
        break;
      case opcode::fmul: // 0x6a (106)
        mnemonic = "fmul";
        break;
      case opcode::dmul: // 0x6b (107)
        mnemonic = "dmul";
        break;
      case opcode::idiv: // 0x6c (108)
        mnemonic = "idiv";
        break;
      case opcode::ldiv: // 0x6d (109)
        mnemonic = "ldiv";
        break;
      case opcode::fdiv: // 0x6e (110)
        mnemonic = "fdiv";
        break;
      case opcode::ddiv: // 0x6f (111)
        mnemonic = "ddiv";
        break;
      case opcode::irem: // 0x70 (112)
        mnemonic = "irem";
        break;
      case opcode::lrem: // 0x71 (113)
        mnemonic = "lrem";
        break;
      case opcode::frem: // 0x72 (114)
        mnemonic = "frem";
        break;
      case opcode::drem: // 0x73 (115)
        mnemonic = "drem";
        break;
      case opcode::ineg: // 0x74 (116)
        mnemonic = "ineg";
        break;
      case opcode::lneg: // 0x75 (117)
        mnemonic = "lneg";
        break;
      case opcode::fneg: // 0x76 (118)
        mnemonic = "fneg";
        break;
      case opcode::dneg: // 0x77 (119)
        mnemonic = "dneg";
        break;
      case opcode::ishl: // 0x78 (120)
        mnemonic = "ishl";
        break;
      case opcode::lshl: // 0x79 (121)
        mnemonic = "lshl";
        break;
      case opcode::ishr: // 0x7a (122)
        mnemonic = "ishr";
        break;
      case opcode::lshr: // 0x7b (123)
        mnemonic = "lshr";
        break;
      case opcode::iushr: // 0x7c (124)
        mnemonic = "iushr";
        break;
      case opcode::lushr: // 0x7d (125)
        mnemonic = "lushr";
        break;
      case opcode::iand: // 0x7e (126)
        mnemonic = "iand";
        break;
      case opcode::land: // 0x7f (127)
        mnemonic = "land";
        break;
      case opcode::ior: // 0x80 (128)
        mnemonic = "ior";
        break;
      case opcode::lor: // 0x81 (129)
        mnemonic = "lor";
        break;
      case opcode::ixor: // 0x82 (130)
        mnemonic = "ixor";
        break;
      case opcode::lxor: // 0x83 (131)
        mnemonic = "lxor";
        break;
      case opcode::iinc: // 0x84 (132)
        mnemonic = "iinc";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        va += appendOperand< int8_t>(map, va, chars, operands);
        break;
      case opcode::i2l: // 0x85 (133)
        mnemonic = "i2l";
        break;
      case opcode::i2f: // 0x86 (134)
        mnemonic = "i2f";
        break;
      case opcode::i2d: // 0x87 (135)
        mnemonic = "i2d";
        break;
      case opcode::l2i: // 0x88 (136)
        mnemonic = "l2i";
        break;
      case opcode::l2f: // 0x89 (137)
        mnemonic = "l2f";
        break;
      case opcode::l2d: // 0x8a (138)
        mnemonic = "l2d";
        break;
      case opcode::f2i: // 0x8b (139)
        mnemonic = "f2i";
        break;
      case opcode::f2l: // 0x8c (140)
        mnemonic = "f2l";
        break;
      case opcode::f2d: // 0x8d (141)
        mnemonic = "f2d";
        break;
      case opcode::d2i: // 0x8e (142)
        mnemonic = "d2i";
        break;
      case opcode::d2l: // 0x8f (143)
        mnemonic = "d2l";
        break;
      case opcode::d2f: // 0x90 (144)
        mnemonic = "d2f";
        break;
      case opcode::i2b: // 0x91 (145)
        mnemonic = "i2b";
        break;
      case opcode::i2c: // 0x92 (146)
        mnemonic = "i2c";
        break;
      case opcode::i2s: // 0x93 (147)
        mnemonic = "i2s";
        break;
      case opcode::lcmp: // 0x94 (148)
        mnemonic = "lcmp";
        break;
      case opcode::fcmpl: // 0x95 (149)
        mnemonic = "fcmpl";
        break;
      case opcode::fcmpg: // 0x96 (150)
        mnemonic = "fcmpg";
        break;
      case opcode::dcmpl: // 0x97 (151)
        mnemonic = "dcmpl";
        break;
      case opcode::dcmpg: // 0x98 (152)
        mnemonic = "dcmpg";
        break;
      case opcode::ifeq: // 0x99 (153)
        mnemonic = "ifeq";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ifne: // 0x9a (154)
        mnemonic = "ifne";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::iflt: // 0x9b (155)
        mnemonic = "iflt";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ifge: // 0x9c (156)
        mnemonic = "ifge";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ifgt: // 0x9d (157)
        mnemonic = "ifgt";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ifle: // 0x9e (158)
        mnemonic = "ifle";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmpeq: // 0x9f (159)
        mnemonic = "if_icmpeq";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmpne: // 0xa0 (160)
        mnemonic = "if_icmpne";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmplt: // 0xa1 (161)
        mnemonic = "if_icmplt";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmpge: // 0xa2 (162)
        mnemonic = "if_icmpge";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmpgt: // 0xa3 (163)
        mnemonic = "if_icmpgt";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_icmple: // 0xa4 (164)
        mnemonic = "if_icmple";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_acmpeq: // 0xa5 (165)
        mnemonic = "if_acmpeq";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::if_acmpne: // 0xa6 (166)
        mnemonic = "if_acmpne";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::goto_: // 0xa7 (167)
        mnemonic = "goto";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::jsr: // 0xa8 (168)
        mnemonic = "jsr";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ret: // 0xa9 (169)
        mnemonic = "ret";
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::tableswitch: // 0xaa (170)
        mnemonic = "tableswitch";
        va += appendTableswitch(map, va, chars, operands);
        break;
      case opcode::lookupswitch: // 0xab (171)
        mnemonic = "lookupswitch";
        va += appendLookupswitch(map, va, chars, operands);
        break;
      case opcode::ireturn: // 0xac (172)
        mnemonic = "ireturn";
        break;
      case opcode::lreturn: // 0xad (173)
        mnemonic = "lreturn";
        break;
      case opcode::freturn: // 0xae (174)
        mnemonic = "freturn";
        break;
      case opcode::dreturn: // 0xaf (175)
        mnemonic = "dreturn";
        break;
      case opcode::areturn: // 0xb0 (176)
        mnemonic = "areturn";
        break;
      case opcode::return_: // 0xb1 (177)
        mnemonic = "return";
        break;
      case opcode::getstatic: // 0xb2 (178)
        mnemonic = "getstatic";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::putstatic: // 0xb3 (179)
        mnemonic = "putstatic";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::getfield: // 0xb4 (180)
        mnemonic = "getfield";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::putfield: // 0xb5 (181)
        mnemonic = "putfield";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::invokevirtual: // 0xb6 (182)
        mnemonic = "invokevirtual";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::invokespecial: // 0xb7 (183)
        mnemonic = "invokespecial";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::invokestatic: // 0xb8 (184)
        mnemonic = "invokestatic";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::invokeinterface: // 0xb9 (185)
        mnemonic = "invokeinterface";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        va += appendOperand<uint8_t>(map, va, chars, operands);
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::invokedynamic: // 0xba (186)
        mnemonic = "invokedynamic";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        va += appendOperand<uint8_t>(map, va, chars, operands);
        va += appendOperand<uint8_t>(map, va, chars, operands);
        break;
      case opcode::new_: // 0xbb (187)
        mnemonic = "new";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::newarray: // 0xbc (188)
        mnemonic = "newarray";
        va += appendOperand<uint8_t>(map, va, chars, operands); // atype
        break;
      case opcode::anewarray: // 0xbd (189)
        mnemonic = "anewarray";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::arraylength: // 0xbe (190)
        mnemonic = "arraylength";
        break;
      case opcode::athrow: // 0xbf (191)
        mnemonic = "athrow";
        break;
      case opcode::checkcast: // 0xc0 (192)
        mnemonic = "checkcast";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::instanceof: // 0xc1 (193)
        mnemonic = "instanceof";
        va += appendOperand<uint16_t>(map, va, chars, operands);
        break;
      case opcode::monitorenter: // 0xc2 (194)
        mnemonic = "monitorenter";
        break;
      case opcode::monitorexit: // 0xc3 (195)
        mnemonic = "monitorexit";
        break;
      case opcode::wide: { // 0xc4 (196)
        mnemonic = "wide";
        // Peek at the wide opcode (the instruction that is wide)
        uint8_t insn = static_cast<uint8_t>(JvmInstructionKind::unknown);
        size_t nRead = map->at(va).limit(1).require(MemoryMap::READABLE).read(&insn).size();
        if (0 == nRead) {
          throw Disassembler::Exception("short read", va);
        }
        va += appendOperand<uint8_t>(map, va, chars, operands); // instruction
        va += appendOperand<uint16_t>(map, va, chars, operands); // index
        // opcode::iinc == // 0x84 (132)
        if (insn == 132) {
          va += appendOperand<int16_t>(map, va, chars, operands); // constant
        }
        break;
      }
      case opcode::multianewarray: // 0xc5 (197)
        mnemonic = "multianewarray";
        va += appendOperand<uint16_t>(map, va, chars, operands); // index
        va += appendOperand<uint8_t>(map, va, chars, operands); // dimensions
        break;
      case opcode::ifnull: // 0xc6 (198)
        mnemonic = "ifnull";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
      case opcode::ifnonnull: // 0xc7 (199)
        mnemonic = "ifnonnull";
        va += appendOperand<int16_t>(map, va, chars, operands);
        break;
// TODO: Following needs test
      case opcode::goto_w: // 0xc8 (200)
        mnemonic = "goto_w";
        va += appendOperand<int32_t>(map, va, chars, operands);
        break;
      case opcode::jsr_w: // 0xc9 (201)
        mnemonic = "jsr_w";
        va += appendOperand<int32_t>(map, va, chars, operands);
        break;

// 6.2 Reserved Opcodes (should not be encountered in valid class file)
      case opcode::breakpoint: // 0xca (202)
        mnemonic = "breakpoint";
        break;
      case opcode::impdep1: // 0xfe (254)
        mnemonic = "impdep1";
        break;
      case opcode::impdep2: // 0xff (255)
        mnemonic = "impdep2";
        break;

      default:
        cout << "Disassembler::Jvm::disassembleOne: Warning, unknown instruction kind " << (int) kind << endl;
        kind = opcode::unknown;
  }

  if (kind == opcode::unknown) {
    delete operands;
    return makeUnknownInstruction(
                 Disassembler::Exception("unknown", start,
                           SgUnsignedCharList((const unsigned char*)&jbc, (const unsigned char*)&jbc+1),
                           0));
  }

  insn = new SgAsmJvmInstruction(start, architecture()->name(), mnemonic, kind);
  insn->set_operandList(operands);
  operands->set_parent(insn);
  insn->set_rawBytes(chars);

#if DEBUG_ON
  cout << "... insn: " << (int)kind << ": " << insn->get_mnemonic()
       << " nOperands:" << insn->nOperands() << ":chars:";
  for (size_t i = 0; i < chars.size(); i++) {
    cout << (int)chars[i] << ":";
  }
  cout << endl;
  for (auto op : insn->get_operandList()->get_operands()) {
    if (op->asUnsigned()) {
      cout << "      unsigned operand:" << *(op->asUnsigned()) << endl;
    }
    else if (op->asSigned()) {
      cout << "       signed operand:" << *(op->asSigned()) << endl;
    }
  }
#endif

  ROSE_ASSERT(((va-start) == chars.size()) && "disassembleOne count error");

  return insn;
}

SgAsmInstruction*
Jvm::makeUnknownInstruction(const Disassembler::Exception &e) {
    SgAsmInstruction *insn = new SgAsmJvmInstruction(e.ip, architecture()->name(), "unknown", opcode::unknown);
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_rawBytes(e.bytes);
    return insn;
}

} // namespace
} // namespace
} // namespace

#endif
