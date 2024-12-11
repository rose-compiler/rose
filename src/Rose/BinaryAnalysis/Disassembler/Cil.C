#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Cil.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmCilInstruction.h>
#include <SgAsmFloatValueExpression.h>
#include <SgAsmIntegerValueExpression.h>

#include <SageBuilderAsm.h>

using std::hex;
using std::dec;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

using namespace ByteOrder;
using namespace Rose::Diagnostics; // mlog WARN, ...

Cil::Cil(const Architecture::Base::ConstPtr &arch)
    : Base(arch) {}

Cil::Ptr
Cil::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Cil(arch));
}

Cil::~Cil() {}

Disassembler::Base::Ptr
Cil::clone() const {
    return Ptr(new Cil(architecture()));
}

SgAsmInstruction*
Cil::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet*)
{
    constexpr bool TRACE_DECODING = false;
    constexpr size_t BUF_SIZE = 11;

    if (va % instructionAlignment_ != 0) {
        throw Disassembler::Exception("instruction is not properly aligned", va);
    }

    // Copy instruction from the memory map
    uint8_t buf[BUF_SIZE]; // largest possible instruction (other than switch)
    size_t nbytes = map->at(va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf).size();
    if (0 == nbytes)
        throw Exception("short read", va);

    // Clear remaining buffer bytes
    for (size_t i = nbytes; i < sizeof buf; ++i) buf[i] = 0;

    SgAsmCilInstruction *insn{nullptr};
    SgAsmIntegerValueExpression* operand{nullptr};

    ASSERT_require(BUF_SIZE >= 10); // BUF_SIZE-1?
    SgUnsignedCharList rawBytes(buf+0, buf+9); //TODO use BUF_SIZE

    uint8_t valueU8{0};
    int32_t token{0}; // metadata token, encoded as a 4-byte integer.

 // Switch over opcode values (Warning: the switch is not over CilInstructionKind)
    switch (rawBytes[0]) {
        case 0x00: // Cil_nop: do nothing
            insn = makeInstruction(va, rawBytes, 1, Cil_nop);
            break;

        case 0x01: // Cil_break: inform a debugger that a breakpoint has been reached
           insn = makeInstruction(va, rawBytes, 1, Cil_break);
           break;

        case 0x02: // Cil_ldarg_0: load argument 0 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_0);
            break;

        case 0x03: // Cil_ldarg_1: load argument 1 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_1);
            break;

        case 0x04: // Cil_ldarg_2: load argument 2 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_2);
            break;

        case 0x05: // Cil_ldarg_3: load argument 3 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_3);
            break;

        case 0x06: // Cil_ldloc_0: load local variable 0 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_0);
            break;

        case 0x07: // Cil_ldloc_1: load local variable 1 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_1);
            break;

        case 0x08: // Cil_ldloc_2: load local variable 2 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_2);
            break;

        case 0x09: // Cil_ldloc_3: load local variable 3 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_3);
            break;

        case 0x0A: // Cil_stloc_0: pop a value from stack into local variable 0
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_0);
            break;

        case 0x0B: // Cil_stloc_1: pop a value from stack into local variable 1
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_1);
            break;

        case 0x0C: // Cil_stloc_2: pop a value from stack into local variable 2
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_2);
            break;

        case 0x0D: // Cil_stloc_3: pop a value from stack into local variable 3
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_3);
            break;

        case 0x0E: // Cil_ldarg_s: load argument numbered num onto the stack, short form
            valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldarg_s, operand);
            break;

        case 0x0F: // Cil_ldarga_s: fetch the address of argument argNum, short form
            valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldarga_s, operand);
            break;

        case 0x10:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_starg_s instruction (store value to the argument numbered num) \n");

             valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueU8(valueU8);
             insn = makeInstruction(va, rawBytes, 2, Cil_starg_s, operand);
             break;
           }

        case 0x11: // Cil_ldloc_s: load local variable of index indx onto stack, short form
            valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldloc_s, operand);
            break;

        case 0x12: // Cil_ldloca_s: load address of local variable with index indx, short form
            valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldloca_s, operand);
            break;

        case 0x13: // Cil_stloc_s: pop a value from stack into local variable indx, short form
            valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_stloc_s, operand);
            break;

        case 0x14: // Cil_ldnull: push a null reference on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldnull);
           break;

        case 0x15: // Cil_ldc_i4_m1: push -1 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_m1);
           break;

        case 0x16: // Cil_ldc_i4_0: push 0 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_0);
           break;

        case 0x17: // Cil_ldc_i4_1: push 1 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_1);
           break;

        case 0x18: // Cil_ldc_i4_2: push 2 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_2);
           break;

        case 0x19: // Cil_ldc_i4_3: push 3 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_3);
           break;

        case 0x1A: // Cil_ldc_i4_4: push 4 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_4);
           break;

        case 0x1B: // Cil_ldc_i4_5: push 5 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_5);
           break;

        case 0x1C: // Cil_ldc_i4_6: push 6 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_6);
           break;

        case 0x1D: // Cil_ldc_i4_7: push 7 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_7);
           break;

        case 0x1E: // Cil_ldc_i4_8: push 8 onto the stack as int32
           insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_8);
           break;

        case 0x1F: { // Cil_ldc_i4_s: push num onto the stack as int32, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_ldc_i4_s, operand);
           break;
        }

        case 0x20: { // Cil_ldc_i4: push num of type int32 onto the stack as int32
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_ldc_i4, operand);
           break;
        }

        case 0x21: { // Cil_ldc_i8: push num of type int64 onto the stack as int64
           int64_t value = ByteOrder::leToHost(*((int64_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI64(value);
           insn = makeInstruction(va, rawBytes, 9, Cil_ldc_i8, operand);
           break;
        }

        case 0x22: { // Cil_ldc_r4: push num of type float32 onto the stack as F
           uint32_t integer_value = ByteOrder::leToHost(*((uint32_t*)(rawBytes.data()+1)));
           float_t* fptr = (float_t*)(&integer_value);
           SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary32(*fptr);
           insn = makeInstruction(va, rawBytes, 5, Cil_ldc_r4, operand);
           break;
        }

        case 0x23: { // Cil_ldc_r8: push num of type float64 onto the stack as F
           uint64_t integer_value = ByteOrder::leToHost(*((uint64_t*)(rawBytes.data()+1)));
           double_t* dptr = (double_t*)(&integer_value);
           SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary64(*dptr);
           insn = makeInstruction(va, rawBytes, 9, Cil_ldc_r8, operand);
           break;
        }

        case 0x25: // Cil_dup: duplicate the value on the top of the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_dup);
           break;

        case 0x26: // Cil_pop: pop value from the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_pop);
           break;

        case 0x27: // Cil_jmp: exit current method and jump to the specified method
           token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(token);
           insn = makeInstruction(va, rawBytes, 5, Cil_jmp, operand);
           break;

        case 0x28: // Cil_call: call method described by method
             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_call, operand);
             break;

        case 0x29: // Cil_calli: call method indicated on the stack with arguments described by callsitedescr
           token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(token);
           insn = makeInstruction(va, rawBytes, 5, Cil_calli, operand);
           break;

        case 0x2A: // Cil_ret: return from method, possibly with a value
           insn = makeInstruction(va, rawBytes, 1, Cil_ret);
           break;

        case 0x2B: { // Cil_br_s: branch to target
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_br_s, operand);
           break;
        }

        case 0x2C: { // Cil_brfalse_s: branch to target if value is zero (false), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_brfalse_s, operand);
           break;
        }

        case 0x2D: { // Cil_brtrue_s: branch to target if value is non-zero (true), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_brtrue_s, operand);
           break;
        }

        case 0x2E: { // Cil_beq_s: branch to target if equal, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_beq_s, operand);
           break;
        }

        case 0x2F: { // Cil_bge_s: branch to target if greater than or equal to, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_bge_s, operand);
           break;
        }

        case 0x30: { // Cil_bgt_s: branch to target if greater than, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_bgt_s, operand);
           break;
        }

        case 0x31: { // Cil_ble_s: branch to target if less than or equal to
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_ble_s, operand);
           break;
        }

        case 0x32: { // Cil_blt_s: branch to target if less than, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_blt_s, operand);
           break;
        }

        case 0x33: { // Cil_bne_un_s: branch to target if unequal or unordered, short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_bne_un_s, operand);
           break;
        }

        case 0x34: { // Cil_bge_un_s: Branch to target if greater than or equal to (unsigned or unordered), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_bge_un_s, operand);
           break;
        }

        case 0x35: { // Cil_bgt_un: branch to target if greater than (unsigned or unordered), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_bgt_un, operand);
           break;
        }

        case 0x36: { // Cil_ble_un_s: branch to target if less than or equal to (unsigned or unordered), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_ble_un_s, operand);
           break;
        }

        case 0x37: { // Cil_blt_un_s: branch to target if less than (unsigned or unordered), short form
           int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI8(value);
           insn = makeInstruction(va, rawBytes, 2, Cil_blt_un_s, operand);
           break;
        }

        case 0x38: { // Cil_br: branch to target
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_br, operand);
           break;
        }

        case 0x39: { // Cil_brfalse: branch to target if value is zero (false)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_brfalse, operand);
           break;
        }

        case 0x3A: { // Cil_brtrue: branch to target if value is non-zero (true)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_brtrue, operand);
           break;
        }

        case 0x3B: { // Cil_beq: branch to target if equal
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_beq, operand);
           break;
        }

        case 0x3C: { // Cil_bge: branch to target if greater than or equal to
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_bge, operand);
           break;
        }

        case 0x3D: { // Cil_bgt: branch to target if greater than
             int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bgt, operand);
             break;
           }

        case 0x3E: { // Cil_ble: branch to target if less than or equal
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_ble, operand);
           break;
        }

        case 0x3F: { // Cil_blt: branch to target if less than
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_blt, operand);
           break;
        }

        case 0x40: { // Cil_bne_un: branch to target if unequal or unordered
             int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bne_un, operand);
             break;
           }

        case 0x41: { // Cil_bge_un: branch to target if greater than or equal to (unsigned or unordered)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_bge_un, operand);
           break;
        }

        case 0x42: { // Cil_bgt_un: branch to target if greater than (unsigned or unordered)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_bgt_un, operand);
           break;
        }

        case 0x43: { // Cil_ble_un: Branch to target if less than or equal to (unsigned or unordered)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_ble_un, operand);
           break;
        }

        case 0x44: { // Cil_blt_un: branch to target if less than (unsigned or unordered)
           int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
           operand = SageBuilderAsm::buildValueI32(value);
           insn = makeInstruction(va, rawBytes, 5, Cil_blt_un, operand);
           break;
        }

        case 0x45: { // Cil_switch: jump to one of n values
           const std::int32_t minSwitchSize = 1 + sizeof(uint32_t);
           const uint32_t nTargets = ByteOrder::leToHost(*((uint32_t*)(rawBytes.data()+1)));

           ASSERT_require(std::numeric_limits<std::uint32_t>::max() / sizeof(std::int32_t) >= nTargets);

           // This maximum number of targets is purely arbitrary. Something is needed for error recovery,
           // for example, this was inserted because nTargets was wrong because of an error disassembling
           // another instruction. Now it fails gracefully with an unknown instruction.
           if (nTargets > 1024) {
             std::cerr << "!!!!---------------------------!!!!\n";
             std::cerr << "Cil_switch instruction has nTargets too large=" << nTargets << "\n";
             insn = makeUnknownInstruction(va, rawBytes, minSwitchSize);
             break;
           }

           operand = SageBuilderAsm::buildValueU32(nTargets);
           insn = makeInstruction(va, rawBytes, minSwitchSize, Cil_switch, operand);

           const std::uint32_t branchTableSize = sizeof(std::int32_t)*nTargets;

           rawBytes.resize(minSwitchSize+branchTableSize);
           std::uint8_t* const loc = rawBytes.data() + minSwitchSize;
           const rose_addr_t brtbl_va = va + minSwitchSize;
           const std::uint32_t bytesRead = map->at(brtbl_va).limit(branchTableSize)
                                               .require(MemoryMap::EXECUTABLE).read(loc).size();
           ASSERT_require(bytesRead == branchTableSize);

           SgAsmOperandList* oplst = insn->get_operandList();
           ASSERT_not_null(oplst);

           for (uint32_t i = 0; i < nTargets; ++i) {
             const int32_t entryofs = minSwitchSize + i * sizeof(std::int32_t);
             const int32_t jmpofs = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+entryofs)));
             oplst->appendOperand(SageBuilderAsm::buildValueI32(jmpofs));
           }

           insn->set_rawBytes(rawBytes);
           ASSERT_require(insn->get_size() == rawBytes.size());
           break;
        }

        case 0x46:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i1 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i1);
             break;
           }

        case 0x47:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_u1 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u1);
             break;
           }

        case 0x48:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i2 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i2);
             break;
           }

        case 0x49:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_u2 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u2);
             break;
           }

        case 0x4A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i4 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i4);
             break;
           }

        case 0x4B: // Cil_ldind_u4: ndirect load value of type unsigned int32 as int32 on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u4);
           break;

        case 0x4C: // Cil_ldind_i8: indirect load value of type int64 as int64 on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i8);
           break;

        case 0x4D: // Cil_ldind_i: indirect load value of type native int as native int on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i);
           break;

        case 0x4E: // Cil_ldind_r4: indirect load value of type float32 as F on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_r4);
           break;

        case 0x4F: // Cil_ldind_r8: indirect load value of type float64 as F on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_r8);
           break;

        case 0x50: // Cil_ldind_ref: indirect load value of type object ref as O on the stack
           insn = makeInstruction(va, rawBytes, 1, Cil_ldind_ref);
           break;

        case 0x51:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_ref instruction (store value of type object ref (type o) into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_ref);
             break;
           }

        case 0x52:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i1 instruction (store value of type int8 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i1);
             break;
           }

        case 0x53:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i2 instruction (store value of type int16 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i2);
             break;
           }

        case 0x54:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i4 instruction (store value of type int32 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i4);
             break;
           }

        case 0x55:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i8 instruction (store value of type int64 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i8);
             break;
           }

        case 0x56:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_r4 instruction (store value of type float32 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_r4);
             break;
           }

        case 0x57:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_r8 instruction (store value of type float64 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_r8);
             break;
           }

        case 0x58: // Cil_add: add two values, returning a new value
            insn = makeInstruction(va, rawBytes, 1, Cil_add);
            break;

        case 0x59: // Cil_sub: subtract value2 from value1, returning a new value
            insn = makeInstruction(va, rawBytes, 1, Cil_sub);
            break;

        case 0x5A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul instruction (multiply values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul);
             break;
           }

        case 0x5B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_div instruction (divide values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_div);
             break;
           }

        case 0x5C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_div_un instruction (divide values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_div_un);
             break;
           }

        case 0x5D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_rem instruction (remiander values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_rem);
             break;
           }

        case 0x5E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_rem_un instruction (remainder values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_rem_un);
             break;
           }

        case 0x5F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_and instruction (and values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_and);
             break;
           }

        case 0x60:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_or instruction (or values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_or);
             break;
           }

        case 0x61:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_xor instruction (or values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_xor);
             break;
           }

        case 0x62:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shl instruction (shift an integer left (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shl);
             break;
           }

        case 0x63:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shr instruction (shift an integer right (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shr);
             break;
           }

        case 0x64:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shr_un instruction (shift an integer right (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shr_un);
             break;
           }

        case 0x65:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_neg instruction (negates value from stack and pushes new value onto the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_neg);
             break;
           }

        case 0x66:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_not instruction (bitwise complement) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_not);
             break;
           }

        case 0x67: // Cil_conv_i1: convert to int8, pushing int32 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1);
           break;

        case 0x68: // Cil_conv_i2: convert to int16, pushing int32 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_i2);
           break;

        case 0x69: // Cil_conv_i4: convert to int32, pushing int32 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_i4);
           break;

        case 0x6A: // Cil_conv_i8: convert to int64, pushing int64 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_i8);
           break;

        case 0x6B: // Cil_conv_r4: convert to float32, pushing F on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_r4);
           break;

        case 0x6C: // Cil_conv_r8: convert to float64, pushing F on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_r8);
           break;

        case 0x6D: // Cil_conv_u4: convert to unsigned int32, pushing int32 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_u4);
           break;

        case 0x6E: // Cil_conv_u8: convert to unsigned int64, pushing int64 on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_u8);
           break;

        case 0x6F: // Cil_callvirt: call a method associated with an object
            token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI32(token);
            insn = makeInstruction(va, rawBytes, 5, Cil_callvirt, operand);
            break;

        case 0x70:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_cpobj instruction (copy a value type from src to dist) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_cpobj, operand);
             break;
           }

        case 0x71:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldobj instruction (copy a valuestored at address src to the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldobj, operand);
             break;
           }

        case 0x72:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldstr instruction (push a string object for the literal string) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldstr, operand);
             break;
           }

        case 0x73: // Cil_newobj: allocate an uninitialized object or value type and call ctor
            token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI32(token);
            insn = makeInstruction(va, rawBytes, 5, Cil_newobj, operand);
            break;

        case 0x74:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_castclass instruction (cast obj to a typeTok) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_castclass, operand);
             break;
           }

        case 0x75:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_isinst instruction (test if obj is an instance of typeTok, returning null or an instance of theat class or interface) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_isinst, operand);
             break;
           }

        case 0x76:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_r_un instruction (data conversion unsigned integer, to floating-point,pushing F on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_r_un);
             break;
           }

     // Note: instruction 0x77 - 0x78 are not legal instructions.

        case 0x79:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_unbox instruction (extract a value-type from obj, its boxed representation) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_unbox, operand);
             break;
           }

        case 0x7A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_throw instruction (throw an exception) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_throw);
             break;
           }

        case 0x7B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldfld instruction (push the address of field of object (or value type), onto the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldfld, operand);
             break;
           }

        case 0x7C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldflda instruction (push the address of field of object (or value type), onto the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldflda, operand);
             break;
           }

        case 0x7D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stfld instruction (replace the value of field of the object obj with value) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stfld, operand);
             break;
           }

        case 0x7E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldsfld instruction (push the value of field on the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldsfld, operand);
             break;
           }

        case 0x7F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldsflda instruction (push the address of the static field, field, on the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldsflda, operand);
             break;
           }

        case 0x80:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stsfld instruction (replace the value of filed with val) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stsfld, operand);
             break;
           }

        case 0x81:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stobj instruction (store a value of type typeTok at an address) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stobj, operand);
             break;
           }

        case 0x82:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i1_un instruction (unsigned data conversion to int8 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i1_un);
             break;
           }

        case 0x83:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i2_un instruction (unsigned data conversion to int16 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i2_un);
             break;
           }

        case 0x84:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i4_un instruction (unsigned data conversion to int32 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i4_un);
             break;
           }

        case 0x85:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i8_un instruction (unsigned data conversion to int64 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i8_un);
             break;
           }

        case 0x86:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u1_un instruction (unsigned data conversion to unsigned int8 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u1_un);
             break;
           }

        case 0x87:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u2_un instruction (unsigned data conversion to unsigned int16 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u2_un);
             break;
           }

        case 0x88:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u4_un instruction (unsigned data conversion to unsigned int32 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u4_un);
             break;
           }

        case 0x89:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u8_un instruction (unsigned data conversion to unsigned int64 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u8_un);
             break;
           }

        case 0x8A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i_un instruction (unsigned data conversion to native int (on stack as native int) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i_un);
             break;
           }

        case 0x8B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u_un instruction (unsigned data conversion to native unsigned int (on stack as native int) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u_un);
             break;
           }

        case 0x8C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_box instruction (convert a boxable value to its boxed form) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_box, operand);
             break;
           }

        case 0x8D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_newarr instruction (convert a new array with elements of type etype) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_newarr, operand);
             break;
           }

        case 0x8E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldlen instruction (push the length (of type native unsigned int) or array on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldlen);
             break;
           }

        case 0x8F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldlema instruction (load the address of element at index onto the top of the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldelema, operand);
             break;
           }

        case 0x90:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i1 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i1);
             break;
           }

        case 0x91:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u1 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u1);
             break;
           }

        case 0x92:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i2 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i2);
             break;
           }

        case 0x93:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u2 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u2);
             break;
           }

        case 0x94:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i4 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i4);
             break;
           }

        case 0x95:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u4 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u4);
             break;
           }

        case 0x96:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i8 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i8);
             break;
           }

        case 0x97:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i instruction (load the element with type native into the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i);
             break;
           }

        case 0x98:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_r4 instruction (load numeric constant) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_r4);
             break;
           }

        case 0x99:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_r8 instruction (load numeric constant) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_r8);
             break;
           }

        case 0x9A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_ref instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_ref);
             break;
           }

        case 0x9B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i);
             break;
           }

        case 0x9C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i1 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i1);
             break;
           }

        case 0x9D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i2 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i2);
             break;
           }

        case 0x9E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i4 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i4);
             break;
           }

        case 0x9F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i8 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i8);
             break;
           }

        case 0xA0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_r4 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_r4);
             break;
           }

        case 0xA1:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_r8 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_r8);
             break;
           }

        case 0xA2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_ref instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_ref);
             break;
           }

        case 0xA3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem instruction (load the element at index onto the top of the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldelem, operand);
             break;
           }

        case 0xA4:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem instruction (replace array element at index with the value on the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stelem, operand);
             break;
           }

        case 0xA5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_unbox_any instruction (extract a value-type from obj, its boxed representation) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_unbox_any, operand);
             break;
           }

     // Note: instruction 0xA6 - 0xAF are not legal instructions.
     // Note: instruction 0xB0 - 0xB2 are not legal instructions.

        case 0xB3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i1 instruction (convert to an int8 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i1);
             insn->set_rawBytes(rawBytes);
             break;
           }

        case 0xB4:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u1 instruction (convert to an unsigned int8 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u1);
             break;
           }

        case 0xB5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i2 instruction (convert to an int16 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i2);
             break;
           }

        case 0xB6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u2 instruction (convert to an unsigned int16 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u2);
             break;
           }

        case 0xB7:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i4 instruction (convert to an int32 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i4);
             break;
           }

        case 0xB8:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u4 instruction (convert to an unsigned int32 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u4);
             break;
           }

        case 0xB9:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i8 instruction (convert to an int64 (on the stack as a int64) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i8);
             break;
           }

        case 0xBA:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u8 instruction (convert to an unsigned int64 (on the stack as a int64) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u8);
             break;
           }

     // Note: instruction 0xBB - 0xBF are not legal instructions.
     // Note: instruction 0xC0 - 0xC1 are not legal instructions.

        case 0xC2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_refanyval instruction (push the address stored in a typedef reference) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_refanyval, operand);
             break;
           }

        case 0xC3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ckfinite instruction (throw ArithmeticException if value is not a finite number) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ckfinite);
             break;
           }

     // Note: instruction 0xC4 - 0xC5 are not legal instructions.

        case 0xC6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mkrefany instruction (push a typed reference ot ptr of type class onto the stack) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_mkrefany);
             break;
           }

     // Note: instruction 0xC7 - 0xCF are not legal instructions.

        case 0xD0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldtoken instruction (convert metadata token to its runtime representation) \n");

             token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldtoken, operand);
             break;
           }

        case 0xD1:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u2 instruction (data conversion to unsigned int16, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u2);
             break;
           }

        case 0xD2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u1 instruction (data conversion to unsigned int8, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u1);
             break;
           }

        case 0xD3: // Cil_conv_i: convert to native int, pushing native int on stack
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_i);
           break;

        case 0xD4: // Cil_conv_ovf_i: convert to a native int (on the stack as a native int) and throw exception on overflow
           insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i);
           break;

        case 0xD5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u instruction (convert to a native unsigned int (on the stack as a native int) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u);
             break;
           }

        case 0xD6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_add_ovf instruction (add signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_add_ovf);
             break;
           }

        case 0xD7:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_add_ovf_un instruction (add unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_add_ovf_un);
             break;
           }

        case 0xD8:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul_ovf instruction (multiply signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul_ovf);
             break;
           }

        case 0xD9:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul_ovf_un instruction (multiply unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul_ovf_un);
             break;
           }

        case 0xDA:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_sub_ovf instruction (subtract signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_sub_ovf);
             break;
           }

        case 0xDB:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_sub_ovf_un instruction (subtract unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_sub_ovf_un);
             break;
           }

        case 0xDC:
           {
          // Note that this is aliased to the endfault instruction.
             if (TRACE_DECODING)
               printf ("Found Cil_endfinally (endfault) instruction (end finally clause of an exception block) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_endfinally);
             break;
           }

        case 0xDD: { // Cil_leave: exit a protected region of code
             int32_t value = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_leave, operand);
             break;
           }

        case 0xDE: { // Cil_leave_s: exit a protected region of code, short form
             int8_t value = ByteOrder::leToHost(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_leave_s, operand);
             break;
           }

        case 0xDF:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i instruction (store value of native int into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i);
             break;
           }

        case 0xE0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u instruction (data conversion native unsigned int, pushing native int on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u);
             break;
           }

// Start of two-byte instructions

        case 0xFE:
           {
             switch (rawBytes[1])
                {
                  case 0x00: // Cil_arglist: return argument list handle for the current method
                     insn = makeInstruction(va, rawBytes, 2, Cil_arglist);
                     break;

                  case 0x01: // Cil_ceq: push 1 (of type int32) if value 1 equals value2, else push 0
                     insn = makeInstruction(va, rawBytes, 2, Cil_ceq);
                     break;

                  case 0x02: // Cil_cgt: push 1 (of type int32) if value1 > value2, else push 0
                      insn = makeInstruction(va, rawBytes, 2, Cil_cgt);
                      break;

                  case 0x03: // Cil_cgt_un: push 1 (of type int32) if value1 > value2, unsigned or unordered, else push 0
                     insn = makeInstruction(va, rawBytes, 2, Cil_cgt_un);
                     break;

                  case 0x04: // Cil_clt: push 1 (of type int32) if value1 < value2, else push 0
                      insn = makeInstruction(va, rawBytes, 2, Cil_clt);
                      break;

                  case 0x05: // Cil_clt_un: push 1 (of type int32) if value1 < value2, unsigned or unordered, else push 0
                     insn = makeInstruction(va, rawBytes, 2, Cil_clt_un);
                     break;

                  case 0x06: // Cil_ldftn: push a pointer to a method referenced by method, on the stack
                     token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueI32(token);
                     insn = makeInstruction(va, rawBytes, 6, Cil_ldftn, operand);
                     break;

                  case 0x07: // Cil_ldvirtftn: push address of virtual method method on the stack
                     token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueI32(token);
                     insn = makeInstruction(va, rawBytes, 6, Cil_ldvirtftn, operand);
                     break;

               // Note: instruction 0xFE08 is an unused instruction

                  case 0x09: { // Cil_ldarg: load argument numbered num onto the stack
                     uint16_t value = ByteOrder::leToHost(*((uint16_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU16(value);
                     insn = makeInstruction(va, rawBytes, 4, Cil_ldarg, operand);
                     break;
                  }

                  case 0x0A: { // Cil_ldarga: fetch the address of the argument argNum
                     uint16_t value = ByteOrder::leToHost(*((uint16_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU16(value);
                     insn = makeInstruction(va, rawBytes, 4, Cil_ldarga, operand);
                     break;
                  }

                  case 0x0B: { // Cil_starg: store value to the argument numbered num
                     uint16_t value = ByteOrder::leToHost(*((uint16_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU16(value);
                     insn = makeInstruction(va, rawBytes, 4, Cil_starg, operand);
                     break;
                  }

                  case 0x0C: { // Cil_ldloc: load local variable of index onto stack
                     uint16_t value = ByteOrder::leToHost(*((uint16_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU16(value);
                     insn = makeInstruction(va, rawBytes, 4, Cil_ldloc, operand);
                     break;
                  }

                  case 0x0D: // Cil_ldloca: load address of local variable with index indx
                     valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU8(valueU8);
                     insn = makeInstruction(va, rawBytes, 3, Cil_ldloca, operand);
                     break;

                  case 0x0E: { // Cil_stloc: pop a value from stack into local variable indx
                     uint16_t value = ByteOrder::leToHost(*((uint16_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU16(value);
                     insn = makeInstruction(va, rawBytes, 4, Cil_stloc, operand);
                     break;
                  }

                  case 0x0F: // Cil_localloc: allocate space from the local memory pool
                     insn = makeInstruction(va, rawBytes, 2, Cil_localloc);
                     break;

               // Note: instruction 0xFE10 is not a legal instruction

                  case 0x11: // Cil_endfilter: end an exception handling filter clause
                     insn = makeInstruction(va, rawBytes, 2, Cil_endfilter);
                     break;

                  case 0x12: // Cil_unaligned_: subsequence pointer instruction might be unaligned
                     valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU8(valueU8);
                     insn = makeInstruction(va, rawBytes, 3, Cil_unaligned_, operand);
                     break;

                  case 0x13: // Cil_volatile_: subsequent pointer reference is volatile
                     insn = makeInstruction(va, rawBytes, 2, Cil_volatile_);
                     break;

                  case 0x14: // Cil_tail_: subsequent call terminates current method
                     insn = makeInstruction(va, rawBytes, 2, Cil_tail_);
                     break;

                  case 0x15: // Cil_initobj: initialize the value at address dest
                     token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueI32(token);
                     insn = makeInstruction(va, rawBytes, 6, Cil_initobj, operand);
                     break;

                  case 0x16: // Cil_constrained_ call a virtual method on a type constrained to be type T
                     token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueI32(token);
                     insn = makeInstruction(va, rawBytes, 6, Cil_constrained_, operand);
                     break;

                  case 0x17: // Cil_cpblk: copy data from memory to memory
                     insn = makeInstruction(va, rawBytes, 2, Cil_cpblk);
                     break;

                  case 0x18: // Cil_initblk: set all bytes in a block of memory to a given byte value
                     insn = makeInstruction(va, rawBytes, 2, Cil_initblk);
                     break;

                  case 0x19: // Cil_no_: the specified fault check(s) normally performed as part of the execution of the subsequent instruction can/shall be skipped
                     valueU8 = ByteOrder::leToHost(*((uint8_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueU8(valueU8);
                     insn = makeInstruction(va, rawBytes, 3, Cil_no_, operand);
                     break;

                  case 0x1A: // Cil_rethrow: rethrow the current exception
                     insn = makeInstruction(va, rawBytes, 2, Cil_rethrow);
                     break;

                  case 0x1C: // Cil_sizeof: push the size, in bytes, of a type as an unsigned int32
                     token = ByteOrder::leToHost(*((int32_t*)(rawBytes.data()+2)));
                     operand = SageBuilderAsm::buildValueI32(token);
                     insn = makeInstruction(va, rawBytes, 6, Cil_sizeof, operand);
                     break;

                  case 0x1D: // Cil_refanytype: push the type token stored in a typed reference
                    insn = makeInstruction(va, rawBytes, 2, Cil_refanytype);
                    break;

                  case 0x1E: // Cil_readonly_ specify that the subsequent array address operation performs no type check at runtime, and that it returns a controlled-mutability managed pointer
                     insn = makeInstruction(va, rawBytes, 2, Cil_readonly_);
                     break;

                  case 0x08: // Cil_unused56
                  case 0x10: // Cil_unused57
                  case 0x1B: // Cil_unused
                  case 0x1F: // Cil_unused53
                  case 0x20: // Cil_unused54
                  case 0x21: // Cil_unused55
                  case 0x22: // Cil_unused70

                  case 0x23 ... 0xFF:
                     mlog[WARN] << "Illegal or unused opcode: 0x" << hex << rawBytes[0] << rawBytes[1] << dec << "\n";
                     insn = makeUnknownInstruction(va, rawBytes, 2);
                     break;

                  default:
                     mlog[FATAL] << "Opcode using prefix 0xFE not handled: 0x" << hex << rawBytes[0] << rawBytes[1] << dec << "\n";
                     ROSE_ABORT();
                     break;
                }
              break;  
           }

// Start of mono two-byte opcodes, 0xF000...0xF022
        case 0xF0: // Cil_unused41: could be two-byte mono opcode!
           mlog[WARN] << "Mono two-byte encoding: 0x" << hex << rawBytes[0] << rawBytes[1] << dec << "\n";
           insn = makeUnknownInstruction(va, rawBytes, 2);
           break;

// Back to one-byte opcodes
        case 0x24:          // Cil_unused99
        case 0x77:          // Cil_unused58
        case 0x78:          // Cil_unused1
        case 0xA6 ... 0xAF: // Cil_unused{5...14}
        case 0xB0 ... 0xB2: // Cil_unused{15...17}
        case 0xBB ... 0xBF: // Cil_unused{50,18...21}
        case 0xC0 ... 0xC1: // Cil_unused{22...23}
        case 0xC4 ... 0xC5: // Cil_unused{24...25}
        case 0xC7 ... 0xCF: // Cil_unused{59...67}
        case 0xE1 ... 0xEF: // Cil_unused{26...40}
        case 0xF1 ... 0xF7: // Cil_unused{42...48}
        case 0xF8 ... 0xFD: // Cil_prefix{7...2}
           mlog[WARN] << "illegal or unused one-byte instruction: 0x" << hex << (int)rawBytes[0] << dec << "\n";
           insn = makeUnknownInstruction(va, rawBytes, 1);
           break;

        case 0xFF: // Cil_prefixref
           mlog[WARN] << "found internal instruction Cil_prefixref: " << hex << (int)rawBytes[0] << dec << "\n";
           insn = makeInstruction(va, rawBytes, 1, Cil_prefixref);
           break;

        default:
           mlog[WARN] << "illegal or unused one or two-byte instruction: 0x" << hex << (int)rawBytes[0] << (int)rawBytes[1] << dec << "\n";
           insn = makeUnknownInstruction(va, rawBytes, 1);
           break;
    }

    ASSERT_not_null(insn);
    ASSERT_require(insn->get_rawBytes().size() > 0);

    return insn;
}

SgAsmInstruction*
Cil::makeUnknownInstruction(const Disassembler::Exception &e) {
    auto insn = new SgAsmCilInstruction(e.ip, *architecture()->registrationId(), Cil_unknown_instruction);
    auto operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    if (e.bytes.empty()) {
        SgUnsignedCharList rawBytes(1,'\0'); // we don't know what, but an instruction is never zero bytes wide
        insn->set_rawBytes(rawBytes);
    } else {
        insn->set_rawBytes(e.bytes);
    }
    return insn;
}

SgAsmCilInstruction*
Cil::makeUnknownInstruction(rose_addr_t va, SgUnsignedCharList &bytes, size_t size)
{
    auto insn = new SgAsmCilInstruction(va, *architecture()->registrationId(), Cil_unknown_instruction);
    auto operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    bytes.resize(size);
    insn->set_rawBytes(bytes);

    return insn;
}

SgAsmCilInstruction*
Cil::makeInstruction(rose_addr_t va, SgUnsignedCharList &bytes, size_t size, CilInstructionKind kind,
                     SgAsmExpression* operand) const
{
    ASSERT_forbid2(Cil_unknown_instruction==kind, "should have called make_unknown_instruction instead");

    auto insn = new SgAsmCilInstruction(va, *architecture()->registrationId(), kind);
    auto operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    if (operand) {
      SageBuilderAsm::appendOperand(insn, operand);
    }

    bytes.resize(size);
    insn->set_rawBytes(bytes);

    // Sanity checks while implementing
    ASSERT_require(insn->get_size() > 0);
    ASSERT_require(insn->get_size() == bytes.size());
    if (insn->get_size() == 1) {
      ASSERT_require((insn->get_anyKind() ^0xFF00) == bytes[0]);
    }
    if (0xFE == bytes[0]) {
      ASSERT_require((insn->get_anyKind() ^0xFE00) == bytes[1]);
    }

    return insn;
}

} // namespace
} // namespace
} // namespace

#endif
