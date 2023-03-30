#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Disassembler/Cil.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser/Cil.h>

#include <SageBuilderAsm.h>

using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

using namespace ByteOrder;

Cil::Cil() {
    name("cil");
    wordSizeBytes(2);
    byteOrder(ByteOrder::ORDER_MSB);
    registerDictionary(RegisterDictionary::instanceCil());
}

Cil::Ptr
Cil::instance() {
    return Ptr(new Cil);
}

Cil::~Cil() {}

Disassembler::Base::Ptr
Cil::clone() const {
    return Ptr(new Cil);
}

bool
Cil::canDisassemble(SgAsmGenericHeader* header) const {
    return (header->get_sections_by_name("CLR Runtime Header").size() > 0)? true : false;
}

Unparser::BasePtr
Cil::unparser() const {
    return Unparser::Cil::instance();
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
    
    // Clear remaining buffer bytes
    for (size_t i = nbytes; i < sizeof buf; ++i) buf[i] = 0;

    SgAsmCilInstruction *insn{nullptr};
    SgAsmIntegerValueExpression* operand{nullptr};
    SgUnsignedCharList rawBytes(buf+0, buf+9); //TODO use BUF_SIZE

    uint8_t valueU8{0};
    int32_t token{0}; // metadata token, encoded as a 4-byte integer.

 // Switch over opcode values
    switch (rawBytes[0]) {
        case 0x00: // Cil_nop: do nothing
            insn = makeInstruction(va, rawBytes, 1, Cil_nop, "nop");
            break;

        case 0x01:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_break instruction (break) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_break, "break");
             break;
           }

        case 0x02: // Cil_ldarg_0: load argument 0 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_0, "ldarg_0");
            break;

        case 0x03: // Cil_ldarg_1: load argument 1 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_1, "ldarg_1");
            break;

        case 0x04: // Cil_ldarg_2: load argument 2 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_2, "ldarg_2");
            break;

        case 0x05: // Cil_ldarg_3: load argument 3 onto the stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldarg_3, "ldarg_3");
            break;

        case 0x06: // Cil_ldloc_0: load local variable 0 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_0, "ldloc_0");
            break;

        case 0x07: // Cil_ldloc_1: load local variable 1 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_1, "ldloc_1");
            break;

        case 0x08: // Cil_ldloc_2: load local variable 2 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_2, "ldloc_2");
            break;

        case 0x09: // Cil_ldloc_3: load local variable 3 onto stack
            insn = makeInstruction(va, rawBytes, 1, Cil_ldloc_3, "ldloc_3");
            break;

        case 0x0A: // Cil_stloc_0: pop a value from stack into local variable 0
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_0, "stloc_0");
            break;

        case 0x0B: // Cil_stloc_1: pop a value from stack into local variable 1
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_1, "stloc_1");
            break;

        case 0x0C: // Cil_stloc_2: pop a value from stack into local variable 2
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_2, "stloc_2");
            break;

        case 0x0D: // Cil_stloc_3: pop a value from stack into local variable 3
            insn = makeInstruction(va, rawBytes, 1, Cil_stloc_3, "stloc_3");
            break;

        case 0x0E: // Cil_ldarg_s: load argument numbered num onto the stack, short form
            valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldarg_s, "ldarg_s", operand);
            break;

        case 0x0F: // Cil_ldarga_s: fetch the address of argument argNum, short form
            valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldarga_s, "ldarga_s", operand);
            break;

        case 0x10:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_starg_s instruction (store value to the argument numbered num) \n");

             valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueU8(valueU8);
             insn = makeInstruction(va, rawBytes, 2, Cil_starg_s, "starg_s", operand);
             break;
           }

        case 0x11: // Cil_ldloc_s: load local variable of index indx onto stack, short form
            valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldloc_s, "ldloc_s", operand);
            break;

        case 0x12:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldloca_s instruction (load local variable of index onto stack) \n");

             valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueU8(valueU8);
             insn = makeInstruction(va, rawBytes, 2, Cil_ldloca_s, "ldloca_s", operand);
             break;
           }

        case 0x13: // Cil_stloc_s: pop a value from stack into local variable indx, short form
            valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueU8(valueU8);
            insn = makeInstruction(va, rawBytes, 2, Cil_stloc_s, "stloc_s", operand);
            break;

        case 0x14:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldnull instruction (push a null reference on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldnull, "ldnull");
             break;
           }

        case 0x15: // Cil_ldc_i4_m1: push -1 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_m1, "ldc_i4_m1");
            break;

        case 0x16: // Cil_ldc_i4_0: push 0 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_0, "ldc_i4_0");
            break;

        case 0x17: // Cil_ldc_i4_1: push 1 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_1, "ldc_i4_1");
            break;

        case 0x18: // Cil_ldc_i4_2: push 2 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_2, "ldc_i4_2");
            break;

        case 0x19: // Cil_ldc_i4_3: push 3 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_3, "ldc_i4_3");
            break;

        case 0x1A: // Cil_ldc_i4_4: push 4 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_4, "ldc_i4_4");
            break;

        case 0x1B: // Cil_ldc_i4_5: push 5 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_5, "ldc_i4_5");
            break;

        case 0x1C: // Cil_ldc_i4_6: push 6 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_6, "ldc_i4_6");
            break;

        case 0x1D: // Cil_ldc_i4_7: push 7 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_7, "ldc_i4_7");
            break;

        case 0x1E: // Cil_ldc_i4_8: push 8 onto the stack as int32
            insn = makeInstruction(va, rawBytes, 1, Cil_ldc_i4_8, "ldc_i4_8");
            break;

        case 0x1F: { // Cil_ldc_i4_s: push num onto the stack as int32, short form
            int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI8(value);
            insn = makeInstruction(va, rawBytes, 2, Cil_ldc_i4_s, "ldc_i4_s", operand);
            break;
        }

        case 0x20: { // Cil_ldc_i4: push num of type int32 onto the stack as int32
            int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI32(value);
            insn = makeInstruction(va, rawBytes, 5, Cil_ldc_i4, "ldc_i4", operand);
            break;
        }

        case 0x21: { // Cil_ldc_i8: push num of type int64 onto the stack as int64
            int64_t value = ByteOrder::le_to_host(*((int64_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI64(value);
            insn = makeInstruction(va, rawBytes, 9, Cil_ldc_i8, "ldc_i8", operand);
            break;
        }

        case 0x22: { // Cil_ldc_r4: push num of type float32 onto the stack as F
            uint32_t integer_value = ByteOrder::le_to_host(*((uint32_t*)(rawBytes.data()+1)));
            float_t* fptr = (float_t*)(&integer_value);
            SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary32(*fptr);
            insn = makeInstruction(va, rawBytes, 5, Cil_ldc_r4, "ldc_r4", operand);
            break;
        }

        case 0x23: { // Cil_ldc_r8: push num of type float64 onto the stack as F
            uint64_t integer_value = ByteOrder::le_to_host(*((uint64_t*)(rawBytes.data()+1)));
            double_t* dptr = (double_t*)(&integer_value);
            SgAsmFloatValueExpression* operand = SageBuilderAsm::buildValueIeee754Binary64(*dptr);
            insn = makeInstruction(va, rawBytes, 5, Cil_ldc_r8, "ldc_r8", operand);
            break;
        }

        case 0x25:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_dup instruction (duplicate the value on the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 5, Cil_dup, "dup");
             break;
           }

        case 0x26:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_pop instruction (remove the top element from the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_pop, "pop");
             break;
           }

        case 0x27:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_jmp instruction (exit current method and jump to the specified method) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_jmp, "jmp", operand);
             break;
           }

        case 0x28: // Cil_call: call method described by method
             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_call, "call", operand);
             break;

        case 0x29:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_callsitedescr instruction (call method indicated on the stack with arguments described by callsitedescr) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_calli, "calli", operand);
             break;
           }

        case 0x2A: // Cil_ret: return from method, possibly with a value
             insn = makeInstruction(va, rawBytes, 1, Cil_ret, "ret");
             break;

        case 0x2B: { // Cil_br_s: branch to target
            int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI8(value);
            insn = makeInstruction(va, rawBytes, 2, Cil_br_s, "br_s", operand);
            break;
        }

        case 0x2C: { // Cil_brfalse_s: branch to target if value is zero (false), short form
            int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI8(value);
            insn = makeInstruction(va, rawBytes, 2, Cil_brfalse_s, "brfalse_s", operand);
            break;
        }

        case 0x2D: { // Cil_brtrue_s: branch to target if value is non-zero (true), short form
            int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI8(value);
            insn = makeInstruction(va, rawBytes, 2, Cil_brtrue_s, "brtrue_s", operand);
            break;
        }

        case 0x2E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_beq_s instruction (branch to target if equal to) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_beq_s, "beq_s", operand);
             break;
           }

        case 0x2F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bge_s instruction (branch to target if equal) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_bge_s, "bge_s", operand);
             break;
           }

        case 0x30:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bgt_s instruction (branch to target if greater than) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_bgt_s, "bgt_s", operand);
             break;
           }

        case 0x31:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ble_s instruction (branch to target if less than or equal to) \n");
               
             insn = makeInstruction(va, rawBytes, 1, Cil_bgt_s, "bgt_s");
             break;
           }

        case 0x32:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_blt_s instruction (branch to target if less than) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 1, Cil_blt_s, "blt_s", operand);
             break;
           }

        case 0x33:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bne_un_s instruction (branch to target if not equal) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 1, Cil_bne_un_s, "bne_un_s", operand);
             break;
           }

        case 0x34:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bge_un_s instruction (branch to target if greater than or equal) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_bge_un_s, "bge_un_s", operand);
             break;
           }

        case 0x35:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bgt_un instruction (branch on less than, unsigned or unordered) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_bgt_un, "bgt_un", operand);
             break;
           }

        case 0x36:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ble_un_s instruction (branch to target if less than (unsigned or unordered)) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);

             insn = makeInstruction(va, rawBytes, 2, Cil_ble_un_s, "ble_un_s", operand);
             break;
           }

        case 0x37:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_blt_un_s instruction (branch to target if less than (unsigned or unordered)) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_blt_un_s, "blt_un_s", operand);
             break;
           }

        case 0x38:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_br instruction (branch to target) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_br, "br", operand);
             break;
           }

        case 0x39:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_brfalse instruction (branch to target if value is zero (false)) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_brfalse, "brfalse", operand);
             break;
           }

        case 0x3A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_brtrue instruction (branch to target if value is nonzero (true)) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_brtrue, "brtrue", operand);
             break;
           }

        case 0x3B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_beq instruction (branch to target if equal) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_beq, "beq", operand);
             break;
           }

        case 0x3C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bge instruction (branch to target if equal) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bge, "bge", operand);
             break;
           }

        case 0x3D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bgt instruction (branch to target if greater than) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 2, Cil_bgt, "bgt", operand);
             break;
           }

        case 0x3E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ble instruction (branch to target if less than or equal) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_beq, "beq", operand);
             break;
           }

        case 0x3F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_blt instruction (branch to target if less than) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_blt, "blt", operand);
             break;
           }

        case 0x40:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bge_un instruction (branch to target if greater than or equal) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bge_un, "bge_un", operand);
             break;
           }

        case 0x41:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bge_un instruction (branch to target if greater than or equal) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bge_un_s, "bge_un_s", operand);
             break;
           }

        case 0x42:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_bgt_un instruction (branch on greater than, unsigned or unordered) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_bgt_un, "bgt_un", operand);
             break;
           }

        case 0x43:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ble_un instruction (branch on less than, unsigned or unordered) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_ble_un, "ble_un", operand);
             break;
           }

        case 0x44:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_blt_un instruction (branch to target if less than (unsigned or unordered)) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);
             insn = makeInstruction(va, rawBytes, 5, Cil_blt_un, "blt_un", operand);
             break;
           }

        case 0x45:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_switch instruction (jump to one of n values)) \n");

             const std::int32_t MIN_SWITCH_SIZE = 5; 
             
             const uint32_t value_0 = ByteOrder::le_to_host(*((uint32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueU32(value_0);
             
             insn = makeInstruction(va, Cil_switch, "switch", operand);
             
             ASSERT_require(std::numeric_limits<std::uint32_t>::max() / sizeof(std::int32_t) >= value_0);
             const std::uint32_t branchTableSize = sizeof(std::int32_t)*value_0;

             rawBytes.resize(MIN_SWITCH_SIZE+branchTableSize);
             std::uint8_t* const loc = rawBytes.data() + MIN_SWITCH_SIZE;
             const rose_addr_t   brtbl_va = va + MIN_SWITCH_SIZE;
             const std::uint32_t bytesRead = map->at(brtbl_va).limit(branchTableSize)
                                               .require(MemoryMap::EXECUTABLE).read(loc).size();                                               
             ASSERT_require(bytesRead == branchTableSize);
             
             SgAsmOperandList* oplst = insn->get_operandList();
             ASSERT_not_null(oplst);
                        
             for (uint32_t i = 0; i < value_0; ++i) {
               const int32_t entryofs = MIN_SWITCH_SIZE + i * sizeof(std::int32_t);                
               const int32_t jmpofs = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+entryofs)));
               oplst->append_operand(SageBuilderAsm::buildValueI32(jmpofs));  
             }
             
             insn->set_raw_bytes(rawBytes);
             break;
           }

        case 0x46:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i1 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i1, "ldind_i1");
             break;
           }

        case 0x47:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_u1 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u1, "ldind_u1");
             break;
           }

        case 0x48:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i2 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i2, "ldind_i2");
             break;
           }

        case 0x49:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_u2 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u2, "ldind_u2");
             break;
           }

        case 0x4A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i4 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i4, "ldind_i4");
             break;
           }

        case 0x4B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i4 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i4, "ldind_i4");
             break;
           }

        case 0x4C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_u8 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_u8, "ldind_u8");
             break;
           }

        case 0x4D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_i instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_i, "ldind_i");
             break;
           }

        case 0x4E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_r4 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_r4, "ldind_r4");
             break;
           }

        case 0x4F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_r8 instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_r8, "ldind_r8");
             break;
           }

        case 0x50:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldind_ref instruction (load value indirect onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldind_ref, "ldind_ref");
             break;
           }

        case 0x51:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_ref instruction (store value of type object ref (type o) into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_ref, "stind_ref");
             break;
           }

        case 0x52:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i1 instruction (store value of type int8 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i1, "stind_i1");
             break;
           }

        case 0x53:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i2 instruction (store value of type int16 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i2, "stind_i2");
             break;
           }

        case 0x54:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i4 instruction (store value of type int32 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i4, "stind_i4");
             break;
           }

        case 0x55:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i8 instruction (store value of type int64 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i8, "stind_i8");
             break;
           }

        case 0x56:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_r4 instruction (store value of type float32 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_r4, "stind_r4");
             break;
           }

        case 0x57:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_r8 instruction (store value of type float64 into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_r8, "stind_r8");
             break;
           }

        case 0x58: // Cil_add: add two values, returning a new value
            insn = makeInstruction(va, rawBytes, 1, Cil_add, "add");
            break;

        case 0x59: // Cil_sub: subtract value2 from value1, returning a new value
            insn = makeInstruction(va, rawBytes, 1, Cil_sub, "sub");
            break;

        case 0x5A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul instruction (multiply values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul, "mul");
             break;
           }

        case 0x5B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_div instruction (divide values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_div, "div");
             break;
           }

        case 0x5C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_div_un instruction (divide values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_div_un, "div_un");
             break;
           }

        case 0x5D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_rem instruction (remiander values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_rem, "rem");
             break;
           }

        case 0x5E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_rem_un instruction (remainder values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_rem_un, "rem_un");
             break;
           }

        case 0x5F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_and instruction (and values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_and, "and");
             break;
           }

        case 0x60:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_or instruction (or values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_or, "or");
             break;
           }

        case 0x61:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_xor instruction (or values on stack and replace with result onto stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_xor, "xor");
             break;
           }

        case 0x62:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shl instruction (shift an integer left (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shl, "shl");
             break;
           }

        case 0x63:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shr instruction (shift an integer right (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shr, "shr");
             break;
           }

        case 0x64:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_shr_un instruction (shift an integer right (shift in zero), return an integer) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_shr_un, "shr_un");
             break;
           }

        case 0x65:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_neg instruction (negates value from stack and pushes new value onto the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_neg, "neg");
             break;
           }

        case 0x66:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_not instruction (bitwise complement) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_not, "not");
             break;
           }

        case 0x67:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_i1 instruction (data conversion to int8, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1, "conv_i1");
             break;
           }

        case 0x68:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_i2 instruction (data conversion to int16, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1, "conv_i1");
             break;
           }

        case 0x69:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_i4 instruction (data conversion to int32, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1, "conv_i1");
             break;
           }

        case 0x6A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_i8 instruction (data conversion to int64, pushing int64 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1, "conv_i1");
             break;
           }

        case 0x6B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_r4 instruction (data conversion to float32, pushing F on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_i1, "conv_i1");
             break;
           }

        case 0x6C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_r8 instruction (data conversion to float64, pushing F on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_r8, "conv_r8");
             break;
           }

        case 0x6D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u4 instruction (data conversion to int32, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u4, "conv_u4");
             break;
           }

        case 0x6E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u8 instruction (data conversion to unsigned int64, pushing int64 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u8, "conv_u8");
             break;
           }

        case 0x6F: // Cil_callvirt: call a method associated with an object
            token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI32(token);
            insn = makeInstruction(va, rawBytes, 5, Cil_callvirt, "callvirt", operand);
            break;

        case 0x70:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_cpobj instruction (copy a value type from src to dist) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_cpobj, "cpobj", operand);
             break;
           }

        case 0x71:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldobj instruction (copy a valuestored at address src to the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldobj, "ldobj", operand);
             break;
           }

        case 0x72:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldstr instruction (push a string object for the literal string) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldstr, "ldstr", operand);
             break;
           }

        case 0x73: // Cil_newobj: allocate an uninitialized object or value type and call ctor
            token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
            operand = SageBuilderAsm::buildValueI32(token);
            insn = makeInstruction(va, rawBytes, 5, Cil_newobj, "newobj", operand);
            break;

        case 0x74:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_castclass instruction (cast obj to a typeTok) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_castclass, "castclass", operand);
             break;
           }

        case 0x75:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_isinst instruction (test if obj is an instance of typeTok, returning null or an instance of theat class or interface) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_isinst, "isinst", operand);
             break;
           }

        case 0x76:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_r_un instruction (data conversion unsigned integer, to floating-point,pushing F on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_r_un, "conv_r_un");
             break;
           }

     // Note: instruction 0x77 - 0x78 are not legal instructions.

        case 0x79:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_unbox instruction (extract a value-type from obj, its boxed representation) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_unbox, "unbox", operand);
             break;
           }

        case 0x7A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_throw instruction (throw an exception) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_throw, "throw");
             break;
           }

        case 0x7B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldfld instruction (push the address of field of object (or value type), onto the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldfld, "ldfld", operand);
             break;
           }

        case 0x7C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldflda instruction (push the address of field of object (or value type), onto the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldflda, "ldflda", operand);
             break;
           }

        case 0x7D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stfld instruction (replace the value of field of the object obj with value) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stfld, "stfld", operand);
             break;
           }

        case 0x7E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldsfld instruction (push the value of field on the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldsfld, "ldsfld", operand);
             break;
           }

        case 0x7F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldsflda instruction (push the address of the static field, field, on the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldsflda, "ldsflda", operand);
             break;
           }

        case 0x80:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stsfld instruction (replace the value of filed with val) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stsfld, "stsfld", operand);
             break;
           }

        case 0x81:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stobj instruction (store a value of type typeTok at an address) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stobj, "stobj", operand);
             break;
           }

        case 0x82:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i1_un instruction (unsigned data conversion to int8 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i1_un, "conv_ovf_i1_un");
             break;
           }

        case 0x83:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i2_un instruction (unsigned data conversion to int16 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i2_un, "conv_ovf_i2_un");
             break;
           }

        case 0x84:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i4_un instruction (unsigned data conversion to int32 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i4_un, "conv_ovf_i4_un");
             break;
           }

        case 0x85:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i8_un instruction (unsigned data conversion to int64 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i8_un, "conv_ovf_i8_un");
             break;
           }

        case 0x86:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u1_un instruction (unsigned data conversion to unsigned int8 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u1_un, "conv_ovf_u1_un");
             break;
           }

        case 0x87:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u2_un instruction (unsigned data conversion to unsigned int16 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u2_un, "conv_ovf_u2_un");
             break;
           }

        case 0x88:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u4_un instruction (unsigned data conversion to unsigned int32 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u4_un, "conv_ovf_u4_un");
             break;
           }

        case 0x89:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u8_un instruction (unsigned data conversion to unsigned int64 (on stack as int32) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u8_un, "conv_ovf_u8_un");
             break;
           }

        case 0x8A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i_un instruction (unsigned data conversion to native int (on stack as native int) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i_un, "conv_ovf_i_un");
             break;
           }

        case 0x8B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u_un instruction (unsigned data conversion to native unsigned int (on stack as native int) with overflow detection) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u_un, "conv_ovf_u_un");
             break;
           }

        case 0x8C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_box instruction (convert a boxable value to its boxed form) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_box, "box", operand);
             break;
           }

        case 0x8D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_newarr instruction (convert a new array with elements of type etype) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_newarr, "newarr", operand);
             break;
           }

        case 0x8E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldlen instruction (push the length (of type native unsigned int) or array on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldlen, "ldlen");
             break;
           }

        case 0x8F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldlema instruction (load the address of element at index onto the top of the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldelema, "ldelema", operand);
             break;
           }

        case 0x90:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i1 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i1, "ldelem_i1");
             break;
           }

        case 0x91:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u1 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u1, "ldelem_u1");
             break;
           }

        case 0x92:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i2 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i2, "ldelem_i2");
             break;
           }

        case 0x93:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u2 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u2, "ldelem_u2");
             break;
           }

        case 0x94:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i4 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i4, "ldelem_i4");
             break;
           }

        case 0x95:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_u4 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_u4, "ldelem_u4");
             break;
           }

        case 0x96:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i8 instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i8, "ldelem_i8");
             break;
           }

        case 0x97:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_i instruction (load the element with type native into the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_i, "ldelem_i");
             break;
           }

        case 0x98:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_r4 instruction (load numeric constant) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_r4, "ldelem_r4");
             break;
           }

        case 0x99:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_r8 instruction (load numeric constant) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_r8, "ldelem_r8");
             break;
           }

        case 0x9A:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem_ref instruction (load the address of element at index onto the top of the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ldelem_ref, "ldelem_ref");
             break;
           }

        case 0x9B:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i, "stelem_i");
             break;
           }

        case 0x9C:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i1 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i1, "stelem_i1");
             break;
           }

        case 0x9D:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i2 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i2, "stelem_i2");
             break;
           }

        case 0x9E:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i4 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i4, "stelem_i4");
             break;
           }

        case 0x9F:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_i8 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_i8, "stelem_i8");
             break;
           }

        case 0xA0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_r4 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_r4, "stelem_r4");
             break;
           }

        case 0xA1:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_r8 instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_r8, "stelem_r8");
             break;
           }

        case 0xA2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem_ref instruction (replace array element at index with the native int value on the stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stelem_ref, "stelem_ref");
             break;
           }

        case 0xA3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldelem instruction (load the element at index onto the top of the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldelem, "ldelem", operand);
             break;
           }

        case 0xA4:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stelem instruction (replace array element at index with the value on the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_stelem, "stelem", operand);
             break;
           }

        case 0xA5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_unbox_any instruction (extract a value-type from obj, its boxed representation) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_unbox_any, "unbox_any", operand);
             break;
           }

     // Note: instruction 0xA6 - 0xAF are not legal instructions.
     // Note: instruction 0xB0 - 0xB2 are not legal instructions.

        case 0xB3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i1 instruction (convert to an int8 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i1, "conv_ovf_i1");
             insn->set_raw_bytes(rawBytes);
             break;
           }

        case 0xB4:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u1 instruction (convert to an unsigned int8 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u1, "conv_ovf_u1");
             break;
           }

        case 0xB5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i2 instruction (convert to an int16 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i2, "conv_ovf_i2");
             break;
           }

        case 0xB6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u2 instruction (convert to an unsigned int16 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u2, "conv_ovf_u2");
             break;
           }

        case 0xB7:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i4 instruction (convert to an int32 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i4, "conv_ovf_i4");
             break;
           }

        case 0xB8:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u4 instruction (convert to an unsigned int32 (on the stack as a int32) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u4, "conv_ovf_u4");
             break;
           }

        case 0xB9:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i8 instruction (convert to an int64 (on the stack as a int64) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i8, "conv_ovf_i8");
             break;
           }

        case 0xBA:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u8 instruction (convert to an unsigned int64 (on the stack as a int64) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u8, "conv_ovf_u8");
             break;
           }

     // Note: instruction 0xBB - 0xBF are not legal instructions.
     // Note: instruction 0xC0 - 0xC1 are not legal instructions.

        case 0xC2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_refanyval instruction (push the address stored in a typedef reference) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_refanyval, "refanyval", operand);
             break;
           }

        case 0xC3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ckfinite instruction (throw ArithmeticException if value is not a finite number) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_ckfinite, "ckfinite");
             break;
           }

     // Note: instruction 0xC4 - 0xC5 are not legal instructions.

        case 0xC6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mkrefany instruction (push a typed reference ot ptr of type class onto the stack) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_mkrefany, "mkrefany");
             break;
           }

     // Note: instruction 0xC7 - 0xCF are not legal instructions.

        case 0xD0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_ldtoken instruction (convert metadata token to its runtime representation) \n");

             token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(token);
             insn = makeInstruction(va, rawBytes, 5, Cil_ldtoken, "ldtoken", operand);
             break;
           }

        case 0xD1:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u2 instruction (data conversion to unsigned int16, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u2, "conv_u2");
             break;
           }

        case 0xD2:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u1 instruction (data conversion to unsigned int8, pushing int32 on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u1, "conv_u1");
             break;
           }

        case 0xD3:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_i instruction (data conversion to native int, pushing native int on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u2, "conv_u2");
             break;
           }

        case 0xD4:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_i instruction (convert to a native int (on the stack as a native int) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_i, "conv_ovf_i");
             break;
           }

        case 0xD5:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_ovf_u instruction (convert to a native unsigned int (on the stack as a native int) and throw exception on overflow) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_ovf_u, "conv_ovf_u");
             break;
           }

        case 0xD6:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_add_ovf instruction (add signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_add_ovf, "add_ovf");
             break;
           }

        case 0xD7:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_add_ovf_un instruction (add unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_add_ovf_un, "add_ovf_un");
             break;
           }

        case 0xD8:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul_ovf instruction (multiply signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul_ovf, "mul_ovf");
             break;
           }

        case 0xD9:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_mul_ovf_un instruction (multiply unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_mul_ovf_un, "mul_ovf_un");
             break;
           }

        case 0xDA:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_sub_ovf instruction (subtract signed integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_sub_ovf, "sub_ovf");
             break;
           }

        case 0xDB:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_sub_ovf_un instruction (subtract unsigned integer values with overflow check) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_sub_ovf_un, "sub_ovf_un");
             break;
           }

        case 0xDC:
           {
          // Note that this is aliased to the endfault instruction.
             if (TRACE_DECODING)
               printf ("Found Cil_endfinally (endfault) instruction (end finally clause of an exception block) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_endfinally, "endfinally");
             break;
           }

        case 0xDD:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_leave instruction (Exit a protected region of code) \n");

             int32_t value = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI32(value);

             insn = makeInstruction(va, rawBytes, 1, Cil_leave, "leave", operand);
             break;
           }

        case 0xDE:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_leave_s instruction (Exit a protected region of code) \n");

             int8_t value = ByteOrder::le_to_host(*((int8_t*)(rawBytes.data()+1)));
             operand = SageBuilderAsm::buildValueI8(value);

             insn = makeInstruction(va, rawBytes, 1, Cil_leave_s, "leave_s", operand);
             break;
           }

        case 0xDF:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_stind_i instruction (store value of native int into memory at address) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_stind_i, "stind_i");
             break;
           }

        case 0xE0:
           {
             if (TRACE_DECODING)
               printf ("Found Cil_conv_u instruction (data conversion native unsigned int, pushing native int on stack) \n");

             insn = makeInstruction(va, rawBytes, 1, Cil_conv_u, "conv_u");
             break;
           }

        case 0xFE:
           {
             if (TRACE_DECODING)
               printf ("opcode prefix 0xFE: buf[0] = %04x \n",buf[0]);

             switch (rawBytes[1])
                {
                  case 0x00: // Cil_arglist: return argument list handle for the current method
                      insn = makeInstruction(va, rawBytes, 2, Cil_arglist, "arglist");
                      break;

                  case 0x01:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ceq instruction (push 1 (of type int32) if value 1 equals value2, else push 0) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_ceq, "ceq");
                       break;
                     }

                  case 0x02: // Cil_cgt: push 1 (of type int32) if value1 > value2, else push 0
                      insn = makeInstruction(va, rawBytes, 2, Cil_cgt, "cgt");
                      break;

                  case 0x03:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_cgt_un instruction (push 1 (of type uint32) if value1 > value2, else push 0) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_cgt_un, "cgt_un");
                       break;
                     }

                  case 0x04: // Cil_clt: push 1 (of type int32) if value1 < value2, else push 0
                      insn = makeInstruction(va, rawBytes, 2, Cil_clt, "clt");
                      break;

                  case 0x05:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_clt_un instruction (push 1 (of type uint32) if value1 < value2, else push 0) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_clt_un, "clt_un");
                       break;
                     }

                  case 0x06:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldftn instruction (push a pointer to a method referenced by method, on the stack) \n");

                       token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueI32(token);
                       insn = makeInstruction(va, rawBytes, 6, Cil_ldftn, "ldftn", operand);
                       break;
                     }

                  case 0x07:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldvirtftn instruction (push a pointer to a method referenced by method, on the stack) \n");

                       token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueI32(token);
                       insn = makeInstruction(va, rawBytes, 6, Cil_ldvirtftn, "ldvirtftn", operand);
                       break;
                     }

               // Note: instruction 0x08 is not a legal instruction.

                  case 0x09:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldarg instruction (load argument numbered num onto the stack) \n");

                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU16(value);

                       insn = makeInstruction(va, rawBytes, 4, Cil_ldarg_s, "ldarg", operand);
                       break;
                     }

                  case 0x0A:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldarga instruction (fetch the address of the argument argNum) \n");

                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU16(value);
                       insn = makeInstruction(va, rawBytes, 4, Cil_ldarga, "ldarga", operand);
                       break;
                     }

                  case 0x0B: { // Cil_starg: store value to the argument numbered num
                      uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(rawBytes.data()+1)));
                      operand = SageBuilderAsm::buildValueU16(value);
                      insn = makeInstruction(va, rawBytes, 4, Cil_starg, "starg", operand);
                      break;
                  }

                  case 0x0C:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldloc instruction (load local variable of index onto stack) \n");

                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU16(value);
                       insn = makeInstruction(va, rawBytes, 4, Cil_ldloc, "ldloc", operand);
                       break;
                     }

                  case 0x0D:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_ldloca_s instruction (load local variable of index onto stack) \n");

                       valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU8(valueU8);
                       insn = makeInstruction(va, rawBytes, 3, Cil_ldloca_s, "ldloca_s", operand);
                       break;
                     }

                  case 0x0E:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_stloc instruction (pop value from stack to local variable) \n");

                       uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU16(value);

                       insn = makeInstruction(va, rawBytes, 4, Cil_stloc, "stloc", operand);
                       break;
                     }

                  case 0x0F:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_localloc instruction (allocate space from the local memory pool) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_localloc, "localloc");
                       break;
                     }

               // Note: instruction 0x10 is not a legal instruction.

                  case 0x11:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_endfilter instruction (end an exception handling filter clause) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_endfilter, "endfilter");
                       break;
                     }

                  case 0x12:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_unaligned instruction (subsequence pointer instruction might be unaligned) \n");

                       valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU8(valueU8);
                       insn = makeInstruction(va, rawBytes, 3, Cil_unaligned, "unaligned", operand);
                       break;
                     }

                  case 0x13:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_volatile instruction (subsequent pointer reference is volatile) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_volatile, "volatile");
                       break;
                     }

                  case 0x14:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_tail instruction (subsequent call terminates current method) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_tail, "tail");
                       break;
                     }

                  case 0x15:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_initobj instruction (initialize the value at address dest) \n");

                       token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueI32(token);
                       insn = makeInstruction(va, rawBytes, 6, Cil_initobj, "initobj", operand);
                       break;
                     }

                  case 0x16:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_constrained instruction (call a virtual method on a type constrained to be type T) \n");

                       token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueI32(token);
                       insn = makeInstruction(va, rawBytes, 6, Cil_constrained, "constrained", operand);
                       break;
                     }

                  case 0x17:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_cpblk instruction (copy data from memory to memory) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_cpblk, "cpblk");
                       break;
                     }

                  case 0x18:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_initblk instruction (set all bytes in a block of memory to a given byte value) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_initblk, "initblk");
                       break;
                     }

                  case 0x19:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_no instruction (the specified fault check(s) normally performed as part of the execution of the subsequent instruction can/shall be skipped) \n");

                       valueU8 = ByteOrder::le_to_host(*((uint8_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueU8(valueU8);
                       insn = makeInstruction(va, rawBytes, 3, Cil_no, "no", operand);
                       break;
                     }

                  case 0x1A:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_rethrow instruction (rethrow the current exception) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_rethrow, "rethrow");
                       break;
                     }

                  case 0x1C:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_sizeof instruction (push the size, in bytes, of a type as an unsigned int32) \n");

                       token = ByteOrder::le_to_host(*((int32_t*)(rawBytes.data()+1)));
                       operand = SageBuilderAsm::buildValueI32(token);
                       insn = makeInstruction(va, rawBytes, 6, Cil_sizeof, "sizeof", operand);
                       break;
                     }

                  case 0x1D:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_refanytype instruction (push the type token stored in a typed reference) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_refanytype, "refanytype");
                       break;
                     }

                  case 0x1E:
                     {
                       if (TRACE_DECODING)
                         printf ("Found Cil_readonly instruction (specify that the subsequent array address operation performs no type check at runtime, and that it returns a controlled-mutability managed pointer) \n");

                       insn = makeInstruction(va, rawBytes, 2, Cil_readonly, "readonly");
                       break;
                     }

                  case 0x08: // Cil_unused56
                  case 0x10: // Cil_unused57
                  case 0x1B: // Cil_unused
                  case 0x1F: // Cil_unused53
                  case 0x20: // Cil_unused54
                  case 0x21: // Cil_unused55
                  case 0x22: // Cil_unused70
                  case 0x23 ... 0xFF:
                       mprintf ("Error: Illegal or unused opcode: buf[1] = %04x \n", buf[1]);
                       insn = makeUnknownInstruction(va);
                       rawBytes.resize(2);
                       insn->set_raw_bytes(rawBytes);
                       break;

                  default:
                       mprintf ("Fatal: Opcode using prefix 0xFE not handled: buf[1] = %04x \n",buf[1]);
                       ROSE_ABORT();
                       break;
                }
              break;  
           }

        case 0x24: // Cil_unused99
        case 0x77: // Cil_unused58
        case 0x78: // Cil_unused1
        case 0xA6 ... 0xAF: // Cil_unused{5...14}
        case 0xB0 ... 0xB2: // Cil_unused{15...17}
        case 0xBB ... 0xBF: // Cil_unused{50,18...21}
        case 0xC0 ... 0xC1: // Cil_unused{22...23}
        case 0xC4 ... 0xC5: // Cil_unused{24...25}
        case 0xC7 ... 0xCF: // Cil_unused{59...67}
        case 0xE1 ... 0xEF: // Cil_unused{26...40}
        case 0xF0 ... 0xF7: // Cil_unused{41...48}
        case 0xF8 ... 0xFD: // Cil_prefix{7...2}
        case 0xFF:          // Cil_prefixref
        default:
            mprintf ("Error: Illegal or unused opcode: buf[0] = %04x \n", buf[0]);
            insn = makeUnknownInstruction(va);
            rawBytes.resize(1);
            insn->set_raw_bytes(rawBytes);
            break;
    }

    ASSERT_not_null(insn);
    ASSERT_require(insn->get_raw_bytes().size() > 0);

    return insn;
}

SgAsmInstruction*
Cil::makeUnknownInstruction(const Disassembler::Exception &e) {
    SgAsmCilInstruction *insn = new SgAsmCilInstruction(e.ip, "unknown", Cil_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    if (e.bytes.empty()) {
        SgUnsignedCharList rawBytes(1, '\0'); // we don't know what, but an instruction is never zero bytes wide
        insn->set_raw_bytes(rawBytes);
    } else {
        insn->set_raw_bytes(e.bytes);
    }
    return insn;
}

#if 1
SgAsmCilInstruction*
Cil::makeUnknownInstruction(rose_addr_t address) {
    SgAsmCilInstruction *insn = new SgAsmCilInstruction(address, "unknown", Cil_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

 // SgUnsignedCharList rawBytes(1, opt_code);          // we don't know what, but an instruction is never zero bytes wide
 // insn->set_raw_bytes(rawBytes);

    return insn;
}
#endif

SgAsmCilInstruction*
Cil::makeInstruction(rose_addr_t va, CilInstructionKind kind,
                     const std::string &mnemonic, SgAsmExpression* operand) const {
    ASSERT_forbid2(Cil_unknown_instruction==kind, "should have called make_unknown_instruction instead");

    SgAsmCilInstruction *insn = new SgAsmCilInstruction(va, mnemonic, kind);

    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    if (operand) {
        SageBuilderAsm::appendOperand(insn, operand);
    }

    return insn;
}

SgAsmCilInstruction*
Cil::makeInstruction(rose_addr_t va, SgUnsignedCharList &bytes, size_t insnSize,
                     CilInstructionKind kind, const std::string &mnemonic, SgAsmExpression* operand) const {
    auto insn = makeInstruction(va, kind, mnemonic, operand);
    bytes.resize(insnSize);
    insn->set_raw_bytes(bytes);

    return insn;
}

} // namespace
} // namespace
} // namespace

#endif
