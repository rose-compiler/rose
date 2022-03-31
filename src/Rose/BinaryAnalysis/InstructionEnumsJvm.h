#ifndef ROSE_BinaryAnalysis_InstructionEnumsJvm_H
#define ROSE_BinaryAnalysis_InstructionEnumsJvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

/* References:
 *   [1] "The Java® Virtual Machine Specification Java SE 17 Edition"
 *     authors: Tim Lindholm Frank Yellin Gilad Bracha Alex Buckley Daniel Smith
 *     date: 2021-08-09
 */

namespace Rose {
namespace BinaryAnalysis {

/** The Java Virtual Machine (JVM) instruction set */
/**   Chapter 6 [1] */

/** Format of JVM instruction opcode (kind) description
 *
 * mnemonic = opcode (decimal) , opcode (hex) : # operands : [operands] : "Short description of the instruction"
 * --------   ----------------   ------------   ----------   ----------   --------------------------------------
 * nop      = 0                , 0x00         : 0          :            : "Do nothing"
 * ...
 * bipush   = 16               , 0x10         : 1          : byte       : "Push byte"
 * ...
 */

/** JVM instruction types. */
enum class JvmInstructionKind {
  nop             =   0, /**< 0x00:0:: "Do nothing" */
  aconst_null     =   1, /**< 0x01: "Push null" */
  iconst_m1       =   2, /**< 0x02: "Push int constant -1" */
  iconst_0        =   3, /**< 0x03: "Push int constant 0" */
  iconst_1        =   4, /**< 0x04: "Push int constant 1" */
  iconst_2        =   5, /**< 0x05: "Push int constant 2" */
  iconst_3        =   6, /**< 0x06: "Push int constant 3" */
  iconst_4        =   7, /**< 0x07: "Push int constant 4" */
  iconst_5        =   8, /**< 0x08: "Push int constant 5" */
  lconst_0        =   9, /**< 0x09: "Push long constant 0" */
  lconst_1        =  10, /**< 0x0a: "Push long constant 1" */
  fconst_0        =  11, /**< 0x0b: "Push float 0.0" */
  fconst_1        =  12, /**< 0x0c: "Push float 1.0" */
  fconst_2        =  13, /**< 0x0d: "Push float 2.0" */
  dconst_0        =  14, /**< 0x0e: "Push double 0.0" */
  dconst_1        =  15, /**< 0x0f: "Push double 1.0" */
  bipush          =  16, /**< 0x10 1: byte: "Push byte" */
  sipush          =  17, /**< 0x11 2: byte1, byte2: "Push short" */
  ldc             =  18, /**< 0x12 1: index: "Push item from run-time constant pool" */
  ldc_w           =  19, /**< 0x13 2: indexbyte1, indexbyte2: "Push item from run-time constant pool (wide index)" */
  ldc2_w          =  20, /**< 0x14 2: indexbyte1, indexbyte2: "Push long or double from run-time constant pool (wide index)" */
  iload           =  21, /**< 0x15 1: index: "Load int from local variable" */
  lload           =  22, /**< 0x16 1: index: "Load long from local variable" */
  fload           =  23, /**< 0x17 1: index: "Load float from local variable" */
  dload           =  24, /**< 0x18 1: index: "Load double from local variable" */
  aload           =  25, /**< 0x19 1: index: "Load reference from local variable" */
  iload_0         =  26, /**< 0x1a: "Load int from local variable 0" */
  iload_1         =  27, /**< 0x1b: "Load int from local variable 1" */
  iload_2         =  28, /**< 0x1c: "Load int from local variable 2" */
  iload_3         =  29, /**< 0x1d: "Load int from local variable 3" */
  lload_0         =  30, /**< 0x1e: "Load long from local variable 0" */
  lload_1         =  31, /**< 0x1f: "Load long from local variable 1" */
  lload_2         =  32, /**< 0x20: "Load long from local variable 2" */
  lload_3         =  33, /**< 0x21: "Load long from local variable 3" */
  fload_0         =  34, /**< 0x22: "Load float from local variable 0" */
  fload_1         =  35, /**< 0x23: "Load float from local variable 1" */
  fload_2         =  36, /**< 0x24: "Load float from local variable 2" */
  fload_3         =  37, /**< 0x25: "Load float from local variable 3" */
  dload_0         =  38, /**< 0x26: "Load double from local variable 0" */
  dload_1         =  39, /**< 0x27: "Load double from local variable 1" */
  dload_2         =  40, /**< 0x28: "Load double from local variable 2" */
  dload_3         =  41, /**< 0x29: "Load double from local variable 3" */
  aload_0         =  42, /**< 0x2a: "Load reference from local variable 0" */
  aload_1         =  43, /**< 0x2b: "Load reference from local variable 1" */
  aload_2         =  44, /**< 0x2c: "Load reference from local variable 2" */
  aload_3         =  45, /**< 0x2d: "Load reference from local variable 3" */
  iaload          =  46, /**< 0x2e: "Load int from array" */
  laload          =  47, /**< 0x2f: "Load long from array" */
  faload          =  48, /**< 0x30: "Load float from array" */
  daload          =  49, /**< 0x31: "Load double from array" */
  aaload          =  50, /**< 0x32: "Load reference from array" */
  baload          =  51, /**< 0x33: "Load byte or boolean from array" */
  caload          =  52, /**< 0x34: "Load char from array" */
  saload          =  53, /**< 0x35: "Load short from array" */
  istore          =  54, /**< 0x36 1: index "Store int into local variable at index" */
  lstore          =  55, /**< 0x37 1: index */
  fstore          =  56, /**< 0x38 1: index */
  dstore          =  57, /**< 0x39 1: index */
  astore          =  58, /**< 0x3a 1: index */
  istore_0        =  59, /**< 0x3b: "Store int into local variable 0" */
  istore_1        =  60, /**< 0x3c: "Store int into local variable 1" */
  istore_2        =  61, /**< 0x3d: "Store int into local variable 2" */
  istore_3        =  62, /**< 0x3e: "Store int into local variable 3" */
  lstore_0        =  63, /**< 0x3f: "Store long into local variable 0" */
  lstore_1        =  64, /**< 0x40: "Store long into local variable 1" */
  lstore_2        =  65, /**< 0x41: "Store long into local variable 2" */
  lstore_3        =  66, /**< 0x42: "Store long into local variable 3" */
  fstore_0        =  67, /**< 0x43: "Store float into local variable 0" */
  fstore_1        =  68, /**< 0x44: "Store float into local variable 1" */
  fstore_2        =  69, /**< 0x45: "Store float into local variable 2" */
  fstore_3        =  70, /**< 0x46: "Store float into local variable 3" */
  dstore_0        =  71, /**< 0x47: "Store double into local variable 0" */
  dstore_1        =  72, /**< 0x48: "Store double into local variable 1" */
  dstore_2        =  73, /**< 0x49: "Store double into local variable 2" */
  dstore_3        =  74, /**< 0x4a: "Store double into local variable 3" */
  astore_0        =  75, /**< 0x4b: "Store reference into local variable 0" */
  astore_1        =  76, /**< 0x4c: "Store reference into local variable 1" */
  astore_2        =  77, /**< 0x4d: "Store reference into local variable 2" */
  astore_3        =  78, /**< 0x4e: "Store reference into local variable 3" */
  iastore         =  79, /**< 0x4f: "Store into int array" */
  lastore         =  80, /**< 0x50: "Store into long array" */
  fastore         =  81, /**< 0x51: "Store into float array" */
  dastore         =  82, /**< 0x52: "Store into double array" */
  aastore         =  83, /**< 0x53: "Store into reference array" */
  bastore         =  84, /**< 0x54: "Store into byte or boolean array" */
  castore         =  85, /**< 0x55: "Store into char array" */
  sastore         =  86, /**< 0x56: "Store into short array" */
  pop             =  87, /**< 0x57: "Pop the top operand stack value" */
  pop2            =  88, /**< 0x58: "Pop the top one or two operand stack values" */
  dup             =  89, /**< 0x59: "Duplicate the top operand stack value" */
  dup_x1          =  90, /**< 0x5a: "Duplicate the top operand stack value and insert two values down" */
  dup_x2          =  91, /**< 0x5b: "Duplicate the top operand stack value and insert two or three values down" */
  dup2            =  92, /**< 0x5c: "Duplicate the top one or two operand stack values" */
  dup2_x1         =  93, /**< 0x5d: "Duplicate the top one or two operand stack values and insert two or three values down" */
  dup2_x2         =  94, /**< 0x5e: "Duplicate the top one or two operand stack values and insert two, three, or four values down" */
  swap            =  95, /**< 0x5f: "Swap the top two operand stack values" */
  iadd            =  96, /**< 0x60: "Add int" */
  ladd            =  97, /**< 0x61: "Add long" */
  fadd            =  98, /**< 0x62: "Add float" */
  dadd            =  99, /**< 0x63: "Add double" */
  isub            = 100, /**< 0x64: "Subtract int" */
  lsub            = 101, /**< 0x65: "Subtract long" */
  fsub            = 102, /**< 0x66: "Subtract float" */
  dsub            = 103, /**< 0x67: "Subtract double" */
  imul            = 104, /**< 0x68: "Multiply int" */
  lmul            = 105, /**< 0x69: "Multiply long" */
  fmul            = 106, /**< 0x6a: "Multiply float" */
  dmul            = 107, /**< 0x6b: "Multiply double" */
  idiv            = 108, /**< 0x6c: "Divide int" */
  ldiv            = 109, /**< 0x6d: "Divide long" */
  fdiv            = 110, /**< 0x6e: "Divide float" */
  ddiv            = 111, /**< 0x6f: "Divide double" */
  irem            = 112, /**< 0x70: "Remainder int" */
  lrem            = 113, /**< 0x71: "Remainder long" */
  frem            = 114, /**< 0x72: "Remainder float" */
  drem            = 115, /**< 0x73: "Remainder double" */
  ineg            = 116, /**< 0x74: "Negate int" */
  lneg            = 117, /**< 0x75: "Negate long" */
  fneg            = 118, /**< 0x76: "Negate float" */
  dneg            = 119, /**< 0x77: "Negate double" */
  ishl            = 120, /**< 0x78: "Shift left int" */
  lshl            = 121, /**< 0x79: "Shift left long" */
  ishr            = 122, /**< 0x7a: "Shift right int" */
  lshr            = 123, /**< 0x7b: "Shift right long" */
  iushr           = 124, /**< 0x7c: "Logical shift right int" */
  lushr           = 125, /**< 0x7d: "Logical shift right long" */
  iand            = 126, /**< 0x7e: "Boolean AND int" */
  land            = 127, /**< 0x7f: "Boolean AND long" */
  ior             = 128, /**< 0x80: "Boolean OR int" */
  lor             = 129, /**< 0x81: "Boolean OR long" */
  ixor            = 130, /**< 0x82: "Boolean XOR int" */
  lxor            = 131, /**< 0x83: "Boolean XOR long" */
  iinc            = 132, /**< 0x84:2: index, const: "Increment local variable" */
  i2l             = 133, /**< 0x85: "Convert int to long" */
  i2f             = 134, /**< 0x86: "Convert int to float" */
  i2d             = 135, /**< 0x87: "Convert int to double" */
  l2i             = 136, /**< 0x88: "Convert long to int" */
  l2f             = 137, /**< 0x89: "Convert long to float" */
  l2d             = 138, /**< 0x8a: "Convert long to double" */
  f2i             = 139, /**< 0x8b: "Convert float to int" */
  f2l             = 140, /**< 0x8c: "Convert float to long" */
  f2d             = 141, /**< 0x8d: "Convert float to double" */
  d2i             = 142, /**< 0x8e: "Convert double to int" */
  d2l             = 143, /**< 0x8f: "Convert double to long" */
  d2f             = 144, /**< 0x90: "Convert double to float" */
  i2b             = 145, /**< 0x91: "Convert int to byte" */
  i2c             = 146, /**< 0x92: "Convert int to char" */
  i2s             = 147, /**< 0x93: "Convert int to short" */
  lcmp            = 148, /**< 0x94: "Compare long" */
  fcmpl           = 149, /**< 0x95: "Compare float" */
  fcmpg           = 150, /**< 0x96: "Compare float" */
  dcmpl           = 151, /**< 0x97: "Compare double" */
  dcmpg           = 152, /**< 0x98: "Compare double" */
  ifeq            = 153, /**< 0x99 2: branchbyte1, branchbyte2: "Branch if int comparison .eq. with zero succeeds" */
  ifne            = 154, /**< 0x9a 2: branchbyte1, branchbyte2 */
  iflt            = 155, /**< 0x9b 2: branchbyte1, branchbyte2 */
  ifge            = 156, /**< 0x9c 2: branchbyte1, branchbyte2 */
  ifgt            = 157, /**< 0x9d 2: branchbyte1, branchbyte2 */
  ifle            = 158, /**< 0x9e 2: branchbyte1, branchbyte2 */
  if_icmpeq       = 159, /**< 0x9f 2: branchbyte1, branchbyte2 */
  if_icmpne       = 160, /**< 0xa0 2: branchbyte1, branchbyte2 */
  if_icmplt       = 161, /**< 0xa1 2: branchbyte1, branchbyte2 */
  if_icmpge       = 162, /**< 0xa2 2: branchbyte1, branchbyte2 */
  if_icmpgt       = 163, /**< 0xa3 2: branchbyte1, branchbyte2 */
  if_icmple       = 164, /**< 0xa4 2: branchbyte1, branchbyte2: "Branch if int comparison .le. succeeds" */
  if_acmpeq       = 165, /**< 0xa5 2: branchbyte1, branchbyte2 */
  if_acmpne       = 166, /**< 0xa6 2: branchbyte1, branchbyte2 */
  goto_           = 167, /**< 0xa7 2: branchbyte1, branchbyte2 */
  jsr             = 168, /**< 0xa8 2: branchbyte1, branchbyte2 */
  ret             = 169, /**< 0xa9 1: index */
  tableswitch     = 170, /**< 0xaa 16+: [0-3 bytes padding],defaultbyte1,defaultbyte2,defaultbyte3,defaultbyte4,lowbye1,lowbyte2,lowbyte3,lowbyte4,highbyte1,highbyte2,highbyte3,highbyte4,jump offsets... */
  lookupswitch    = 171, /**< 0xab 8+: <0-3 bytes padding>,defaultbyte1,defaultbyte2,defaultbyte3,defaultbyte4,npairs,npairs2,npairs3,npairs4,match-offset pairs... */
  ireturn         = 172, /**< 0xac: "Return int from method" */
  lreturn         = 173, /**< 0xad: "Return long from method" */
  freturn         = 174, /**< 0xae: "Return float from method" */
  dreturn         = 175, /**< 0xaf: "Return double from method" */
  areturn         = 176, /**< 0xb0: "Return reference from method" */
  return_         = 177, /**< 0xb1: "Return void from method" */
  getstatic       = 178, /**< 0xb2 2: indexbyte1, indexbyte2: "Get static field in class" */
  putstatic       = 179, /**< 0xb3 2: indexbyte1, indexbyte2: "Set static field in class" */
  getfield        = 180, /**< 0xb4 2: indexbyte1, indexbyte2: "Fetch field from object" */
  putfield        = 181, /**< 0xb5 2: indexbyte1, indexbyte2: "Set field in object" */
  invokevirtual   = 182, /**< 0xb6 2: indexbyte1, indexbyte2 */
  invokespecial   = 183, /**< 0xb7 2: indexbyte1, indexbyte2: "Invoke instance method; direct invocation of instance initialization methods and methods of the current class and its supertypes" */
  invokestatic    = 184, /**< 0xb8 2: indexbyte1, indexbyte2 */
  invokeinterface = 185, /**< 0xb9 4: indexbyte1, indexbyte2, count, 0 */
  invokedynamic   = 186, /**< 0xba 4: indexbyte1, indexbyte2, 0, 0 */
  new_            = 187, /**< 0xbb 2: indexbyte1, indexbyte2 */
  newarray        = 188, /**< 0xbc 1: atype: "Create new array" */
  anewarray       = 189, /**< 0xbd 2: indexbyte1, indexbyte2: "Create new array of reference" */
  arraylength     = 190, /**< 0xbe: "Get length of array" */
  athrow          = 191, /**< 0xbf: "Throw exception or error" */
  checkcast       = 192, /**< 0xc0 2: indexbyte1, indexbyte2: "Check whether object is of given type" */
  instanceof      = 193, /**< 0xc1 2: indexbyte1, indexbyte2: "Determine if object is of given type" */
  monitorenter    = 194, /**< 0xc2: "Enter monitor for object" */
  monitorexit     = 195, /**< 0xc3: "Exit monitor for object" */
  wide            = 196, /**< 0xc4 3/5: opcode,indexbyte1,indexbyte2 or iinc,indexbyte1,indexbyte2,countbyte1,countbye2 */
  multianewarray  = 197, /**< 0xc5 3: indexbyte1, indexbyte2, dimensions */
  ifnull          = 198, /**< 0xc6 2: branchbyte1, branchbyte2: "Branch if reference is null" */
  ifnonnull       = 199, /**< 0xc7 2: branchbyte1, branchbyte2 */
  goto_w          = 200, /**< 0xc8 4: branchbyte1, branchbyte2, branchbyte3, branchbyte4 */
  jsr_w           = 201, /**< 0xc9 4: branchbyte1, branchbyte2, branchbyte3, branchbyte4 */
  breakpoint      = 202, /**< 0xca */
  impdep1         = 254, /**< 0xfe */
  impdep2         = 255, /**< 0xff */
  unknown         = 666  /**< unknown/illegal opcode */
};

} // namespace
} // namespace

#endif
#endif
